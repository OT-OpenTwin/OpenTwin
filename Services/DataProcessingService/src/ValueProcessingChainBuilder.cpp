#include "ValueProcessingChainBuilder.h"
#include "UnitTokenizer.h"
#include "OTCore/Units/SIUnits.h"
#include "OTCore/Units/ImperialUnits.h"
#include "ValueProcessorMultiply.h"
#include "ValueProcessorAdd.h"
#include "ValueProcessorLog.h"
#include "ValueProcessorPow.h"


ValueProcessing ValueProcessingChainBuilder::build(const std::string& _unitStringCurrent, const std::string& _unitStringTarget)
{
    try
    {
        if (_unitStringCurrent.empty() || _unitStringTarget.empty())
        {
            // Both units have to be dimensionless.
            if (_unitStringCurrent.empty() && _unitStringTarget.empty())
            {
                return ValueProcessing();
            }

            UnitTokenizer tokenizer;
            ResolvedToken dummyToken;
            dummyToken.m_exponent = 1;
            dummyToken.m_resolvedComponent.m_base = { "", "", "SI", ot::Dimension{}, 1.0, 0.0 };

            if (!_unitStringCurrent.empty())
            {
                std::vector<UnitToken> unitTokensCurrent = tokenizer.tokenize(_unitStringCurrent);
                std::vector<ResolvedToken> resolvedTokenCurrent = resolveTokens(unitTokensCurrent);
                assert(resolvedTokenCurrent.size() == 1);
                if (!(resolvedTokenCurrent[0].m_resolvedComponent.m_base.m_dimension == ot::Dimension{}))
                {
                    throw std::exception("Cannot transform a dimensionless unit into a unit with dimension.");
                }
                
                const ValueProcessing processing = buildChain(resolvedTokenCurrent, { dummyToken });
                return processing;
            }
            else
            {
                std::vector<UnitToken> unitTokensTarget = tokenizer.tokenize(_unitStringTarget);
                std::vector<ResolvedToken> resolvedTokenTarget= resolveTokens(unitTokensTarget);
                assert(resolvedTokenTarget.size() == 1);
                if (!(resolvedTokenTarget[0].m_resolvedComponent.m_base.m_dimension == ot::Dimension{}))
                {
                    throw std::exception("Cannot transform a dimensionless unit into a unit with dimension.");
                }

                const ValueProcessing processing = buildChain({ dummyToken }, resolvedTokenTarget);
                return processing;
            }
        }
        else
        {
            UnitTokenizer tokenizer;
            std::vector<UnitToken> unitTokensCurrent = tokenizer.tokenize(_unitStringCurrent);
            std::vector<ResolvedToken> resolvedTokenCurrent = resolveTokens(unitTokensCurrent);
            std::vector<UnitToken> unitTokensTarget = tokenizer.tokenize(_unitStringTarget);

            std::vector<ResolvedToken> resolvedTokenTarget = resolveTokens(unitTokensTarget);

            dimensionCompatibilityGuard(resolvedTokenCurrent, resolvedTokenTarget, _unitStringCurrent, _unitStringTarget);

            const ValueProcessing processing = buildChain(resolvedTokenCurrent, resolvedTokenTarget);
	        return processing;
        }
    }
    catch (std::exception& _e)
    {
        std::string message = "Failed to convert units: " + std::string(_e.what());
        throw std::exception(message.c_str());
    }
}

ValueProcessing ValueProcessingChainBuilder::buildToSIChain(const std::string& _unitExpr)
{
    UnitTokenizer tokenizer;
    std::vector<UnitToken> unitTokens= tokenizer.tokenize(_unitExpr);
    std::vector<ResolvedToken> resolvedTokens = resolveTokens(unitTokens);

    std::list<std::unique_ptr<ValueProcessor>> processors; 


    // ---- Separate log token from linear tokens ----------------------
    const ResolvedToken* logToken = nullptr;
    std::vector<ResolvedToken> linearTokens;

    for (const auto& token : resolvedTokens) 
    {
        if (token.m_resolvedComponent.m_base.isLog() && token.m_exponent == 1)
        {
            logToken = &token;
        }
        else
        {
            linearTokens.push_back(token);
        }
    }

    if (logToken) 
    {
        // ---- dB/Np/... → linear SI ----------------------------------
        // Step 1: inverse-log restores the linear ratio
        auto log = std::make_unique<ValueProcessorLog>(logToken->m_resolvedComponent.m_base.getLogMultiplier(), ValueProcessorLog::SupportedBases::m_10);
        processors.push_back(std::move(log));

        // Step 2: scale the remaining linear tokens to SI base
        // (e.g. the /degF part in dB/degF)
        double factor = netSIScale(linearTokens);
        if (multiplicationIsRelevant(factor))
        {
            auto multipl = std::make_unique<ValueProcessorMultiply>(factor);
            processors.push_back(std::move(multipl));
        }
    }
    else 
    {
        // ---- Pure linear / affine -----------------------------------
        SIBaseConversion si = computeSIBase(linearTokens);

        if (multiplicationIsRelevant(si.scaleFactor))
        {
            auto add = std::make_unique<ValueProcessorMultiply>(si.scaleFactor);
            processors.push_back(std::move(add));
        }

        if (si.isAffine && summationIsRelevant(si.offsetToSI))
        {
            auto multipl = std::make_unique<ValueProcessorMultiply>(si.offsetToSI);
            processors.push_back(std::move(multipl));
        }
    }
    ValueProcessing processing;
    processing.setSequence(std::move(processors));
    return processing;
}

std::vector<ResolvedToken> ValueProcessingChainBuilder::resolveTokens(const std::vector<UnitToken>& _tokens)
{
    std::vector<ResolvedToken> result;
    result.reserve(_tokens.size());
    
    for (const UnitToken& token : _tokens)
    {
        std::optional<ot::ResolvedComponent> resolvedComponent = ot::SIUnits::instance().resolve(token.m_symbol);
        if (!resolvedComponent.has_value())
        {
            resolvedComponent = ot::ImperialUnits::instance().resolve(token.m_symbol);
            if (!resolvedComponent.has_value())
            {
                throw std::exception(("Failed to assign a imperial or si unit to: " + token.m_symbol).c_str());
            }
        }
        ResolvedToken resolvedToken{ resolvedComponent.value() ,token.m_exponent };
        result.push_back(resolvedToken);
    }

    return result;
}


void ValueProcessingChainBuilder::dimensionCompatibilityGuard(std::vector<ResolvedToken>& _tokensTgt, std::vector<ResolvedToken>& _tokensCurrent, const std::string& _currentExpr, const std::string& _tgtExpr)
{
    ot::Dimension srcDim{}, tgtDim{};
    for (auto& token : _tokensCurrent)
    {
        ot::Dimension& srcDimComp = token.m_resolvedComponent.m_base.m_dimension;
        srcDimComp.scaled(token.m_exponent);
        srcDim = srcDim + srcDimComp;
    }

    for (auto& token : _tokensTgt)
    {
        ot::Dimension& tgtDimComp = token.m_resolvedComponent.m_base.m_dimension;
        tgtDimComp.scaled(token.m_exponent);
        tgtDim = tgtDim + tgtDimComp;
    }

    if (!(srcDim == tgtDim))
    {
        throw std::exception(("\"" + _tgtExpr + "\" and \"" + _currentExpr + "\" have different dimensions").c_str());
    }
}

double ValueProcessingChainBuilder::netSIScale(const std::vector<ResolvedToken>& tokens)
{
    double scale = 1.0;
    for (const auto& token : tokens)
    {
        scale *= std::pow(token.m_resolvedComponent.totalScale(), token.m_exponent);
    }
    return scale;
}

ValueProcessing ValueProcessingChainBuilder::buildChain(const std::vector<ResolvedToken>& _current, const std::vector<ResolvedToken>& _tgt)
{
    
    ValueProcessing processing;
    std::list<std::unique_ptr<ValueProcessor>> processors;
            
    // Collect non-log tokens for scale computation
    std::vector<ResolvedToken> srcScale, tgtScale;
    for (const auto& token : _current)
    {
        if (!token.m_resolvedComponent.m_base.isLog()) 
        {
            srcScale.push_back(token);
        }
    }
    for (const auto& token : _tgt)
    {
        if (!token.m_resolvedComponent.m_base.isLog())
        {
            tgtScale.push_back(token);
        }
    }

    const ResolvedToken* srcLog = findLogToken(_current);
    const ResolvedToken* tgtLog = findLogToken(_tgt);
    
    bool srcAffine =  useAffineConversion(srcScale);
    bool tgtAffine =  useAffineConversion(tgtScale);
    
    // Affine conversion only if the affine unit stands alone (no composition). 
    if (srcAffine != tgtAffine)
    {
        //Should also be pretected by the dimension check
        throw std::exception("Affine units must either stand both alone one or be both composed.");
    }

    // ----------------------------------------------------------------
    // CASE 1: linear/degX  ->  linear/degY   (default: affine or scale)
    // CASE 2: linear/degX  ->  dB/degY       (add log at the end)
    // CASE 3: dB/degX      ->  linear/degY   (prepend inverse-log)
    // CASE 4: dB/degX      ->  dB/degY       (cancel log, pure scale)
    // ----------------------------------------------------------------

    if (srcLog && tgtLog) 
    {
        // CASE 4: dB -> dB, log cancels — only scale the non-log part
        double factor = netSIScale(srcScale) / netSIScale(tgtScale);
        // dB(x * factor) = dB(x) + 20*log10(factor) for amplitude
        double dbOffset = tgtLog->m_resolvedComponent.m_base.getLogMultiplier() * std::log10(factor);
        if (summationIsRelevant(dbOffset))
        {
            auto add = std::make_unique<ValueProcessorAdd>(dbOffset);
            processors.push_back(std::move(add));
        }
    }
    else if (!srcLog && tgtLog) 
    {
        // CASE 2: linear -> dB
        // Step A: scale non-log source tokens to SI, then to target scale
        if (srcAffine)
        {
            assert(srcScale.size() == 1 && tgtScale.size() == 1); 
            buildAffineChain(srcScale[0], tgtScale[0], processors);
        }
        else
        {
            double factor = netSIScale(srcScale) / netSIScale(tgtScale);
            if (multiplicationIsRelevant(factor))
            {
                auto add = std::make_unique<ValueProcessorMultiply>(factor);
                processors.push_back(std::move(add));
            }
        }
        auto log = std::make_unique <ValueProcessorLog>(tgtLog->m_resolvedComponent.m_base.getLogMultiplier(), ValueProcessorLog::SupportedBases::m_10);
        processors.push_back(std::move(log));
    }
    else if (srcLog && !tgtLog) {
        // CASE 3: dB -> linear
        // Step A: dB -> linear
        auto pow = std::make_unique <ValueProcessorPow>(srcLog->m_resolvedComponent.m_base.getLogMultiplier(), 10);
        processors.push_back(std::move(pow));

        // Step B: scale to target        
        if (srcAffine && tgtAffine)
        {
            assert(srcScale.size() == 1 && tgtScale.size() == 1);
            buildAffineChain(srcScale[0], tgtScale[0], processors);
        }
        else
        {
            double factor = netSIScale(srcScale) / netSIScale(tgtScale);
            if (multiplicationIsRelevant(factor))
            {
                auto mult = std::make_unique <ValueProcessorMultiply>(factor);
                processors.push_back(std::move(mult));
            }
        }
    }
    else 
    {
        // CASE 1: pure linear/affine (original path)
        if (srcAffine && tgtAffine)
        {
            assert(srcScale.size() == 1 && tgtScale.size() == 1);
            buildAffineChain(srcScale[0], tgtScale[0],processors);
        }
        else
        {
            double factor = netSIScale(srcScale) / netSIScale(tgtScale);
            if (multiplicationIsRelevant(factor))
            {
                auto mult = std::make_unique <ValueProcessorMultiply>(factor);
                processors.push_back(std::move(mult));
            }
        }
        
    }
    processing.setSequence(std::move(processors));

    return processing;
}

void ValueProcessingChainBuilder::buildAffineChain(const ResolvedToken& _current, const ResolvedToken& _tgt, std::list<std::unique_ptr<ValueProcessor>>& _processorChain)
{
    const auto& sourceBase = _current.m_resolvedComponent.m_base;
    const auto& tgtBase = _tgt.m_resolvedComponent.m_base;
    
    // Include prefix factors (e.g. if someone registers mK = milli-kelvin)
    double srcScale = _current.m_resolvedComponent.m_prefix.m_factor * sourceBase.toSIScale;
    double tgtScale = _tgt.m_resolvedComponent.m_prefix.m_factor * tgtBase.toSIScale;

    // Inverse of (x *scale2 + off2) = y -> (y - off2)/ scale2
    // Total equ.: ((a *scale1 + off1) -off2)/ scale2 -> (a*scale1 + off1)/scale2 - off2/scale2 -> a* scale1/scale2 + (off1 - off2)/scale2
    double totalScale = srcScale / tgtScale;
    double offset = (sourceBase.toSIOffset - tgtBase.toSIOffset) / tgtScale;

    
    if (multiplicationIsRelevant(totalScale))
    {
        auto multiply = std::make_unique<ValueProcessorMultiply>(totalScale);
        _processorChain.push_back( std::move(multiply));
    }

    if (summationIsRelevant(offset))
    {
        auto add = std::make_unique<ValueProcessorAdd>(offset);
        _processorChain.push_back(std::move(add));
    }
}

const ResolvedToken* ValueProcessingChainBuilder::findLogToken(const std::vector<ResolvedToken>& _tokens)
{
    for (const auto& t : _tokens)
    {
        if (t.m_resolvedComponent.m_base.isLog())
        {
            if (t.m_exponent == 1)
            {
                return &t;
            }
            else
            {
                throw std::exception("Invalid unit conversion attempted.");
            }
        }
    }
    return nullptr;
}

bool ValueProcessingChainBuilder::useAffineConversion(const std::vector<ResolvedToken>& _tokens)
{
    // Count how many tokens carry affine units
    int affineCount = 0;
    for (const auto& t : _tokens)
    {
        if (t.m_resolvedComponent.m_base.isAffine())
        {
            ++affineCount;
        }
    }

    if (affineCount == 0)
    {
        return false;
    }

    // Affine offset only valid for a single token at exponent exactly +1. 
    // degC -> degF:        Absolute temperature point          -> offset applies
    // degC/m -> degF/km:	Temperature difference / gradient   -> offset does not apply
    // degC*m -> degF*m:    Composed, offset undefined          -> offset does not apply
    // 1/degC -> 1/degF:    Exponent is −1                      -> offset does not apply
    // degC^2 -> degF^2:    Exponent is 2                       -> offset does not apply
    return (_tokens.size() == 1)
        && (affineCount == 1)
        && (_tokens[0].m_exponent == 1);
}

bool ValueProcessingChainBuilder::summationIsRelevant(double _summand)
{
    return (std::abs(_summand) > 1e-15);
}

bool ValueProcessingChainBuilder::multiplicationIsRelevant(double _factor)
{
    return (std::abs(_factor - 1.0) > 1e-15);
}

ValueProcessingChainBuilder::SIBaseConversion ValueProcessingChainBuilder::computeSIBase(const std::vector<ResolvedToken>& tokens)
{
    // Affine: single token, exponent +1
    if (useAffineConversion(tokens)) {
        const auto& token = tokens[0];
        double scale = token.m_resolvedComponent.m_prefix.m_factor * token.m_resolvedComponent.m_base.toSIScale;
        double offset = token.m_resolvedComponent.m_base.toSIOffset;
        return { scale, offset, true };
    }

    // All other cases: pure multiplicative
    return { netSIScale(tokens), 0.0, false };
}

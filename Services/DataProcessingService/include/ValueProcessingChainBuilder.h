#pragma once
#include "OTCore/ValueProcessing/ValueProcessing.h"
#include "ResolvedToken.h"
#include "UnitToken.h"

class ValueProcessingChainBuilder
{
public:
	ot::ValueProcessing build(const std::string& _unitStringCurrent , const std::string& _unitStringTarget);
    ot::ValueProcessing buildToSIChain(const std::string& unitExpr);

private:
	std::vector<ResolvedToken> resolveTokens(const std::vector<UnitToken>& _tokens);
   
	void dimensionCompatibilityGuard(std::vector<ResolvedToken>& _tokensTgt, std::vector<ResolvedToken>& _tokensCurrent, const std::string& _currentExpr, const std::string& _tgtExpr);

    // Computes the net SI scale factor for a list of resolved tokens.
    // scale = product of (prefix.factor * base.toSIScale) ^ exponent
    double netSIScale(const std::vector<ResolvedToken>& tokens);

    ot::ValueProcessing buildChain(const std::vector<ResolvedToken>& _current, const std::vector<ResolvedToken>& _tgt);
    void buildAffineChain(const ResolvedToken& _current, const ResolvedToken& _tgt, ot::ValueProcessing& _processorChain);
    const ResolvedToken* findLogToken(const std::vector<ResolvedToken>& _tokens);
    bool useAffineConversion(const std::vector<ResolvedToken>& _tokens);

    bool summationIsRelevant(double _summand);
    bool multiplicationIsRelevant(double _factor);

    // Computes the net SI scale and SI offset for a single resolved token list.
    // Returns the factor by which the source value must be multiplied to reach
    // the SI base value. For affine units the offset is returned separately.
    struct SIBaseConversion {
        double scaleFactor = 1.0;
        double offsetToSI = 0.0; // added AFTER scaling (affine only)
        bool   isAffine = false;
    };

    SIBaseConversion computeSIBase(const std::vector<ResolvedToken>& tokens);

};

#pragma once
#include "ValueProcessing.h"
#include "ResolvedToken.h"
#include "UnitToken.h"

class ValueProcessingChainBuilder
{
public:
	ValueProcessing build(const std::string& _unitStringCurrent , const std::string& _unitStringTarget);
private:
	std::vector<ResolvedToken> resolveTokens(const std::vector<UnitToken>& _tokens);
   
	void dimensionCompatibilityGuard(std::vector<ResolvedToken>& _tokensTgt, std::vector<ResolvedToken>& _tokensCurrent, const std::string& _currentExpr, const std::string& _tgtExpr);

    // Computes the net SI scale factor for a list of resolved tokens.
    // scale = product of (prefix.factor * base.toSIScale) ^ exponent
    double netSIScale(const std::vector<ResolvedToken>& tokens);

    ValueProcessing buildChain(const std::vector<ResolvedToken>& _current, const std::vector<ResolvedToken>& _tgt);
    void buildAffineChain(const ResolvedToken& _current, const ResolvedToken& _tgt, std::list<std::unique_ptr<ValueProcessor>>& _processorChain);
    const ResolvedToken* findLogToken(const std::vector<ResolvedToken>& _tokens);
    bool useAffineConversion(const std::vector<ResolvedToken>& _tokens);

    bool summationIsRelevant(double _summand);
    bool multiplicationIsRelevant(double _factor);
};

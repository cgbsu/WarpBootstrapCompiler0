#ifndef WARP_BOOTSTRAP_COMPILER_SANITY_CHECK_HPP
#define WARP_BOOTSTRAP_COMPILER_SANITY_CHECK_HPP

template< auto ValueParameterConstant >
struct ValueContainer {
    constinit static auto ValueConstant = ValueParameterConstant;
};

template< auto SearchParameterConstant, auto CurrentTermParameterConstant, auto... TapeParameterConstants >
struct IsInTemplate {
    constexpr static bool IsInCollectionConstant = IsInTemplate< SearchParameterConstant, TapeParameterConstants... >::IsInCollectionConstant;
};

template< auto SearchParameterConstant, auto... TapeParameterConstants >
struct IsInTemplate< SearchParameterConstant, SearchParameterConstant, TapeParameterConstants... > {
    constexpr static bool IsInCollectionConstant = true;
};

template< auto SearchParameterConstant, auto CurrentTermParameterConstant >
struct IsInTemplate< SearchParameterConstant, CurrentTermParameterConstant > {
    constexpr static bool IsInCollectionConstant = false;
};

template< auto... TermParameterConstants >
struct TermBuffer
{
    template< 
            template< auto... > typename TermBufferParameterType, 
            auto... ExtractedParameterConstants 
        >
    constexpr static auto Append( TermBufferParameterType< ExtractedParameterConstants... > ) {
        return TermBuffer< TermParameterConstants..., ExtractedParameterConstants... >{};
    }
    constexpr static auto ToTerms( auto... other_terms ) {
        return ctpg::terms( TermParameterConstants..., other_terms... );
    }
    template< auto TermParameterConstant >
    constexpr static auto GetTerm() {
            static_assert( IsInTemplate< TermParameterConstant, TermParameterConstants... >::IsInCollectionConstant );
            return TermParameterConstant;
    }

};

#endif // WARP_BOOTSTRAP_COMPILER_SANITY_CHECK_HPP

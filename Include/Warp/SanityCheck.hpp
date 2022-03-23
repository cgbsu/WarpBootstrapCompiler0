#ifndef WARP_BOOTSTRAP_COMPILER_SANITY_CHECK_HPP
#define WARP_BOOTSTRAP_COMPILER_SANITY_CHECK_HPP

template< auto ValueParameterConstant >
struct ValueContainer {
    constinit static auto ValueConstant = ValueParameterConstant;
};

template< auto FirstParameterConstant, auto... TapeParameterConstants >
struct TemplateNonTypeParameterTakeOne {
    constexpr static auto first = FirstParameterConstant;
    template< template< auto... > typename RecipticalParameterType, auto... PartialSpecializationParameterConstants >
    using InjectType = RecipticalParameterType< PartialSpecializationParameterConstants..., TapeParameterConstants... >;
};

template< auto SearchParameterConstant, auto... TapeParameterConstants >
struct IsInTemplate
{
    constexpr static bool is_in_collection = false;

    // using test = typename TemplateNonTypeParameterTakeOne< TapeParameterConstants... >
    //                 ::InjectType< IsInTemplate, SearchParameterConstant >;
};

template< auto SearchParameterConstant, auto... TapeParameterConstants >
struct IsInTemplate< SearchParameterConstant, SearchParameterConstant, TapeParameterConstants... > {
    constexpr static bool is_in_collection = true;
};

template< auto SearchParameterConstant, auto CurrentTermParameterConstant >
struct IsInTemplate< SearchParameterConstant, CurrentTermParameterConstant > {
    constexpr static bool is_in_collection = false;
};


template< auto SearchParameterConstant >
struct IsInTemplate< SearchParameterConstant, SearchParameterConstant > {
    constexpr static bool is_in_collection = true;
};


template< auto... TermParameterConstants >
struct TermBuffer
{
    template< 
            template< auto... > typename TermBufferParameterType, 
            auto... ExtractedParameterConstants 
        >
    constexpr static auto append( TermBufferParameterType< ExtractedParameterConstants... > ) {
        return TermBuffer< TermParameterConstants..., ExtractedParameterConstants... >{};
    }
    constexpr static auto to_terms( auto... other_terms ) {
        return ctpg::terms( TermParameterConstants..., other_terms... );
    }
    template< auto TermParameterConstant >
    constexpr static auto get_term() {
            static_assert( IsInTemplate< TermParameterConstant, TermParameterConstants... >::is_in_collection );
            return TermParameterConstant;
    }

};

template< auto CurrentCheckTermParameterConstant, auto... CheckTermParameterConstants >
struct CheckTermsInCollection
{
    template< auto... CollectionParameterConstants >
    constexpr static bool check_terms_in_collection()
    {
        constexpr bool this_in_collection 
                = IsInTemplate< CurrentCheckTermParameterConstant, CollectionParameterConstants... >::is_in_collection;
        static_assert( this_in_collection );
        auto next_in_collection = CheckTermsInCollection< CollectionParameterConstants... >
                ::template check_terms_in_collection< CheckTermParameterConstants... >();
        return this_in_collection && next_in_collection;
    }
};

template< auto CurrentCheckTermParameterConstant >
struct CheckTermsInCollection< CurrentCheckTermParameterConstant >
{
    template< auto... CollectionParameterConstants >
    constexpr static bool check_terms_in_collection()
    {   
        constexpr bool this_in_collection 
                = IsInTemplate< CurrentCheckTermParameterConstant, CollectionParameterConstants... >::is_in_collection;
        static_assert( this_in_collection );
        return this_in_collection;
    }
};

template< 
        auto PrecedenceParameterConstant, 
        auto AssociativityParameterConstant, 
        auto... OperatorParameterConstants 
    >
struct OperatorBuffer : public TermBuffer< OperatorParameterConstants... >
{
    using ThisType = OperatorBuffer< 
            PrecedenceParameterConstant, 
            AssociativityParameterConstant, 
            OperatorParameterConstants... 
        >;
    
    template< 
            auto NextAssociativityParameterConstant, 
            auto... ExtractedParameterConstants 
        >
    constexpr static auto derive()
    {
        return OperatorBuffer< 
                PrecedenceParameterConstant + 1, 
                NextAssociativityParameterConstant, 
                ThisType{}, 
                ExtractedParameterConstants... 
            >{};
    }


    template< 
            auto TransformOperationParameterConstant, 
            auto... ToTransformParameterConstants 
        >
    constexpr static auto transform_terms()
    {
        static_assert( CheckTermsInCollection< ToTransformParameterConstants... >
                ::template check_terms_in_collection< OperatorParameterConstants... >() );
        return OperatorBuffer< 
                PrecedenceParameterConstant, 
                AssociativityParameterConstant, 
                OperatorParameterConstants..., 
                TransformOperationParameterConstant( 
                        ToTransformParameterConstants 
                    )... 
            >{};
    }

};

#endif // WARP_BOOTSTRAP_COMPILER_SANITY_CHECK_HPP

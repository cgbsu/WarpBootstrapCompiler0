#include <Warp/Function.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_TERMS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_TERMS_HPP


namespace Warp::Parser
{
    enum class RegexLiteralTerms
    {
        NaturalNumber = 0, 
        Identifier = 1, 
        BooleanLiteral = 2 
    };

    enum class NonTerminalTerms
    {
        Factor = 0, 
        Sum = 1, 
        ParenthesisScope = 2, 
        Comparison = 3, 
        LogicalOperation = 5, 
        BooleanAnd = 6, 
        BooleanOr = 7, 
        ExtendedBooleanLogic = 8 
    };

    enum class StringTerms
    {
        LetKeyword = 0, 
        And = 1, 
        Or = 2, 
        Not = 3, 
        BiCondition = 4, 
        Implies = 5, 
    };

    #define LITERAL_REGEX_TERM( TYPE, REGEX ) \
        template<> \
        struct Term< RegexLiteralTerms:: TYPE > \
        { \
            constexpr const static char regex[] = REGEX; \
            constexpr const static char name[] = #TYPE ; \
            constexpr const static auto term = ctpg::regex_term< regex >{ name }; \
            constexpr const static auto literal_type = RegexLiteralTerms:: TYPE; \
        }

    #define NON_TERMINAL_TERM( TYPE, STORAGE_TYPE ) \
        template<> \
        struct Term< NonTerminalTerms:: TYPE > \
        { \
            using StorageType = STORAGE_TYPE ; \
            constexpr const static char name[] = #TYPE ; \
            constexpr const static auto term = ctpg::nterm< STORAGE_TYPE >{ name }; \
        }

    #define STRING_TERM( TYPE, STRING ) \
        template<> \
        struct Term< StringTerms:: TYPE > \
        { \
            constexpr const static char name[] = #TYPE ; \
            constexpr const static char string[] = STRING ; \
            constexpr const static auto term = ctpg::string_term{ string }; \
        }


    template< auto LiteralParameterTypeParameterConstant >
    struct Term {};

    template< auto TypeParameterConstant = RegexLiteralTerms::NaturalNumber >
    struct DefaultTypes {
        using Type = size_t;
    };

    // For testing regexes for this program and many others, and for learning, thank you too https://regexr.com/ //
    LITERAL_REGEX_TERM( NaturalNumber, "[0-9][0-9]*" );
    LITERAL_REGEX_TERM( Identifier, "[a-zA-Z\\_][a-zA-Z0-9\\_]*" );
    LITERAL_REGEX_TERM( BooleanLiteral, "(true)|(false)" ); //Originally (true)\\b|(false)\\b, ctpg seems to take care of this. //
    NON_TERMINAL_TERM( Factor, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( Sum, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( ParenthesisScope, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( LogicalOperation, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( Comparison, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( BooleanAnd, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( BooleanOr, Warp::AbstractSyntaxTree::NodeVariantType );
    // STRING_TERM( And, "&&" );
    STRING_TERM( And, "&&" );
    STRING_TERM( Or, "||" );
    STRING_TERM( BiCondition, "<->" );
    STRING_TERM( Implies, "->" );

    template< 
            auto PriorityParameterConstant, 
            typename ParameterType,  
            ParameterType NodeTypeParameterConstant
        >
    struct TermForwarder
    {
        constexpr static const auto term = ctpg::char_term( 
                Warp::Utilities::to_char( NodeTypeParameterConstant ), 
                PriorityParameterConstant, 
                ctpg::associativity::ltor 
            );
    };

    template< 
            auto PriorityParameterConstant, 
            RegexLiteralTerms NodeTypeParameterConstant
        >
    struct TermForwarder< 
                PriorityParameterConstant, 
                RegexLiteralTerms, 
                NodeTypeParameterConstant
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::regex_term< TermDataType::regex >{ 
                TermDataType::name, 
                PriorityParameterConstant  
            };
    };

    template< 
            auto PriorityParameterConstant, 
            NonTerminalTerms NodeTypeParameterConstant
        >
    struct TermForwarder< 
                PriorityParameterConstant, 
                NonTerminalTerms, 
                NodeTypeParameterConstant
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::nterm< typename TermDataType::StorageType >{ 
                TermDataType::name, 
                PriorityParameterConstant  
            };
    };

    template< 
            auto PriorityParameterConstant, 
            StringTerms NodeTypeParameterConstant
        >
    struct TermForwarder< 
                PriorityParameterConstant, 
                StringTerms, 
                NodeTypeParameterConstant
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::string_term{ 
                TermDataType::string, 
                PriorityParameterConstant, 
                ctpg::associativity::ltor 
            };
    };

    // NO PRIORITY //

    enum class TermBuilderType {
        NoPriority
    };

    template< 
            typename ParameterType, 
            ParameterType NodeTypeParameterConstant
        >
    struct TermForwarder< 
                TermBuilderType::NoPriority, 
                ParameterType, 
                NodeTypeParameterConstant
            > {
        constexpr static const auto term = ctpg::char_term( 
                Warp::Utilities::to_char( NodeTypeParameterConstant ) 
            );
    };

    template< RegexLiteralTerms NodeTypeParameterConstant >
    struct TermForwarder< 
                TermBuilderType::NoPriority, 
                RegexLiteralTerms, 
                NodeTypeParameterConstant
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::regex_term< TermDataType::regex >{ TermDataType::name };
    };

    template< 
            NonTerminalTerms NodeTypeParameterConstant
        >
    struct TermForwarder< 
                TermBuilderType::NoPriority, 
                NonTerminalTerms, 
                NodeTypeParameterConstant
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::nterm< typename TermDataType::StorageType >{ TermDataType::name };
    };

    template< StringTerms NodeTypeParameterConstant >
    struct TermForwarder< 
                TermBuilderType::NoPriority, 
                StringTerms, 
                NodeTypeParameterConstant
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::string_term{ TermDataType::string };
    };

    template< 
            auto PriorityParameterConstant, 
            auto TermTypeParameterParameterConstant  
        >
    constexpr auto forward_term()
    {
        return TermForwarder< 
                PriorityParameterConstant, 
                decltype( TermTypeParameterParameterConstant ), 
                TermTypeParameterParameterConstant  
            >::term;
    }

    template< 
            typename PreviousParameterType, 
            auto PriorityParameterConstant, 
            auto TermParameterConstant, 
            bool HasTermParameterConstant = false 
        >
    struct TermGetter {
        constexpr static const auto term = PreviousParameterType::template get_term< TermParameterConstant >();
    };

    template< 
            typename PreviousParameterType, 
            auto PriorityParameterConstant, 
            auto TermParameterConstant 
        >
    struct TermGetter< 
                PreviousParameterType, 
                PriorityParameterConstant, 
                TermParameterConstant, 
                true 
            > {
        constexpr static const auto term = forward_term< PriorityParameterConstant, TermParameterConstant >();
    };
    
    #define SPECIALIZE_TERM_BUILDER( TO_TUPLE, PREVIOUS_TYPE ) \
        using ThisType = TermBuilder< PREVIOUS_TYPE, PriorityParameterConstant, TermParameterConstants... >; \
        template< auto NextPriorityParameterConstant > \
        struct Next \
        { \
            template< auto... NextTermsParameterConstant > \
            using TermsType = TermBuilder<  \
                    ThisType,  \
                    NextPriorityParameterConstant,  \
                    NextTermsParameterConstant...  \
                >; \
        }; \
        template< auto OffsetParameterConstant > \
        constexpr const static auto add_priorities = Warp::Utilities::EnableOperation<  \
                []( auto left, auto right ) { return left + right; },  \
                PriorityParameterConstant,  \
                OffsetParameterConstant,  \
                std::is_same<  \
                        TermBuilderType,  \
                        decltype( OffsetParameterConstant )  \
                    >::value,  \
                OffsetParameterConstant  \
            >::value; \
        template< auto OffsetParameterConstant, auto... NextTermsParameterConstants > \
        using AddToPriority = typename ThisType::Next<  \
                    add_priorities< OffsetParameterConstant > \
                >::TermsType<  \
                    NextTermsParameterConstants...  \
                >; \
        template< auto... NextTermsParameterConstants > \
        using AddOnePriority = AddToPriority< 1, NextTermsParameterConstants... >; \
        template< auto... NextTermsParameterConstants > \
        using NoPriority = typename ThisType::Next<  \
                TermBuilderType::NoPriority  \
            >::TermsType<  \
                    NextTermsParameterConstants...  \
                >; \
        constexpr static auto to_tuple() \
        { \
            TO_TUPLE \
        } \
        template< auto TermParameterConstant > \
        constexpr static auto get_term() \
        { \
            return TermGetter< \
                    PREVIOUS_TYPE, \
                    PriorityParameterConstant, \
                    TermParameterConstant, \
                    Warp::Utilities::HasConstant< \
                            TermParameterConstant, \
                            TermParameterConstants... \
                        >::value \
                >::term; \
        } 

    template< typename PreviousParameterType, auto PriorityParameterConstant, auto... TermParameterConstants >
    struct TermBuilder
    {
        constexpr static const auto priority = PriorityParameterConstant;
        SPECIALIZE_TERM_BUILDER( 
                return std::tuple_cat( 
                        std::tuple( forward_term< PriorityParameterConstant, TermParameterConstants >()... ), 
                        PreviousParameterType::to_tuple() 
                    );, 
                PreviousParameterType 
            )
    };

    template< auto PriorityParameterConstant, auto... TermParameterConstants >
    struct TermBuilder< void, PriorityParameterConstant, TermParameterConstants... >
    {
        SPECIALIZE_TERM_BUILDER( 
                return std::tuple( forward_term< PriorityParameterConstant, TermParameterConstants >()... );, 
                void 
            )
    };


    template< auto StartPriority, auto... TermParameterConstants >
    using SafeTermsType = TermBuilder< void, StartPriority, TermParameterConstants... >;

    template< auto... TermParameterConstants >
    using EasySafeTermsType = SafeTermsType< 0, TermParameterConstants... >;
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_TERMS_HPP

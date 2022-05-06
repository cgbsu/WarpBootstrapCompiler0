#include <Warp/Function.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_TERMS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_TERMS_HPP


namespace Warp::Parser
{
    enum class RegexLiteralTerms {
        NaturalNumber = 0, 
        Identifier = 1
    };

    enum class NonTerminalTerms
    {
        Factor = 0, 
        Sum = 1, 
        ParenthesisScope = 2, 
        Comparison = 3, 
        Negation = 4, 
        LogicalOperation = 5 
    };

    enum class StringTerms
    {
        LetKeyword = 0, 
        And = 1, 
        Or = 2, 
        Not = 3, 
        BiCondition = 4, 
        Implies = 5 
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

    LITERAL_REGEX_TERM( NaturalNumber, "[0-9][0-9]*" );
    LITERAL_REGEX_TERM( Identifier, "[a-zA-Z\\_][a-zA-Z0-9\\_]*" );
    NON_TERMINAL_TERM( Factor, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( Sum, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( ParenthesisScope, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( LogicalOperation, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( Comparison, Warp::AbstractSyntaxTree::NodeVariantType );
    NON_TERMINAL_TERM( Negation, Warp::AbstractSyntaxTree::NodeVariantType );
    STRING_TERM( And, "&&" );
    STRING_TERM( Or, "||" );
    STRING_TERM( BiCondition, "<->" );
    STRING_TERM( Implies, "->" );

    template< 
            auto PriorityParameterConstant, 
            typename ParameterType,  
            ParameterType NodeTypeParameterConstant
            // auto AssociativityParameterConstant = ctpg::associativity::ltor
        >
    struct TermForwarder
    {
        constexpr static const auto term = ctpg::char_term( 
                Warp::Utilities::to_char( NodeTypeParameterConstant ), 
                PriorityParameterConstant, 
                ctpg::associativity::ltor 
                // AssociativityParameterConstant
            );
    };

    template< 
            auto PriorityParameterConstant, 
            RegexLiteralTerms NodeTypeParameterConstant
            // auto AssociativityParameterConstant = ctpg::associativity::ltor 
        >
    struct TermForwarder< 
                PriorityParameterConstant, 
                RegexLiteralTerms, 
                NodeTypeParameterConstant
                // AssociativityParameterConstant 
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::regex_term< TermDataType::regex >{ 
                TermDataType::name, 
                PriorityParameterConstant//, 
                // ctpg::associativity::ltor 
            };
    };

    template< 
            auto PriorityParameterConstant, 
            NonTerminalTerms NodeTypeParameterConstant
            // auto AssociativityParameterConstant = ctpg::associativity::ltor 
        >
    struct TermForwarder< 
                PriorityParameterConstant, 
                NonTerminalTerms, 
                NodeTypeParameterConstant
                // AssociativityParameterConstant 
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::nterm< typename TermDataType::StorageType >{ 
                TermDataType::name, 
                PriorityParameterConstant//, 
            };
    };

    template< 
            auto PriorityParameterConstant, 
            StringTerms NodeTypeParameterConstant
            // auto AssociativityParameterConstant = ctpg::associativity::ltor 
        >
    struct TermForwarder< 
                PriorityParameterConstant, 
                StringTerms, 
                NodeTypeParameterConstant
                // AssociativityParameterConstant 
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
            char NodeTypeParameterConstant
            // auto AssociativityParameterConstant = ctpg::associativity::ltor
        >
    struct TermForwarder< 
                TermBuilderType::NoPriority, 
                char, 
                NodeTypeParameterConstant
                // AssociativityParameterConstant 
            > {
        constexpr static const auto term = ctpg::char_term( 
                Warp::Utilities::to_char( NodeTypeParameterConstant ) 
            );
    };

    template< 
            RegexLiteralTerms NodeTypeParameterConstant 
            // auto AssociativityParameterConstant = ctpg::associativity::ltor 
        >
    struct TermForwarder< 
                TermBuilderType::NoPriority, 
                RegexLiteralTerms, 
                NodeTypeParameterConstant
                // AssociativityParameterConstant 
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::regex_term< TermDataType::regex >{ TermDataType::name };
    };

    template< 
            NonTerminalTerms NodeTypeParameterConstant
            // auto AssociativityParameterConstant = ctpg::associativity::ltor 
        >
    struct TermForwarder< 
                TermBuilderType::NoPriority, 
                NonTerminalTerms, 
                NodeTypeParameterConstant
                // AssociativityParameterConstant 
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::nterm< typename TermDataType::StorageType >{ TermDataType::name };
    };

    template< 
            StringTerms NodeTypeParameterConstant
            // auto AssociativityParameterConstant = ctpg::associativity::ltor 
        >
    struct TermForwarder< 
                TermBuilderType::NoPriority, 
                StringTerms, 
                NodeTypeParameterConstant
                // AssociativityParameterConstant 
            > {
        using TermDataType = Term< NodeTypeParameterConstant >;
        constexpr const static auto term = ctpg::string_term{ TermDataType::string };
    };



    template< 
            auto PriorityParameterConstant, 
            auto TermTypeParameterParameterConstant//, 
            // auto AssociativityParameterConstant = ctpg::associativity::ltor
        >
    auto forward_term()
    {
        return TermForwarder< 
                PriorityParameterConstant, 
                decltype( TermTypeParameterParameterConstant ), 
                // Warp::Utilities::CleanType< 
                        // std::decay_t< decltype( TermTypeParameterParameterConstant ), 
                    // >, 
                TermTypeParameterParameterConstant//, 
                // AssociativityParameterConstant 
            >::term;
    }


    template< typename PreviousParameterType, auto PriorityParameterConstant, auto... TermParameterConstants >
    struct TermBuilder
    {
        constexpr static const auto priority = PriorityParameterConstant;
        using ThisType = TermBuilder< PreviousParameterType, PriorityParameterConstant, TermParameterConstants... >;
        template< auto NextPriorityParameterConstant = PriorityParameterConstant + 1 >
        struct Next
        {
            template< auto... NextTermsParameterConstant >
            using TermsType = TermBuilder< 
                    ThisType, 
                    NextPriorityParameterConstant, 
                    NextTermsParameterConstant... 
                >;
        };
        constexpr static auto to_tuple()
        {
            return std::tuple_cat( 
                    std::tuple( forward_term< PriorityParameterConstant, TermParameterConstants >()... ), 
                    PreviousParameterType::to_tuple() 
                );
        }
    };


    template< auto PriorityParameterConstant, auto... TermParameterConstants >
    struct TermBuilder< 
            void, 
            PriorityParameterConstant, 
            TermParameterConstants... 
        >
    {
        constexpr static const auto priority = PriorityParameterConstant;
        using ThisType = TermBuilder< void, PriorityParameterConstant, TermParameterConstants... >;
        template< auto NextPriorityParameterConstant = PriorityParameterConstant + 1 >
        struct Next
        {
            template< auto... NextTermsParameterConstant >
            using TermsType = TermBuilder< 
                    ThisType, 
                    NextPriorityParameterConstant, 
                    NextTermsParameterConstant... 
                >;
        };
        constexpr static auto to_tuple() {
            return std::tuple( forward_term< PriorityParameterConstant, TermParameterConstants >()... );
        }
    };
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_TERMS_HPP

/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parserFile.y"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int yylex(void);
void yyerror(char const *);

FILE *inFile;
FILE *outFile;

#line 83 "parserFile.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUMBER = 258,                  /* NUMBER  */
    SIGN_NUMBER = 259,             /* SIGN_NUMBER  */
    FLOAT = 260,                   /* FLOAT  */
    IDENT = 261,                   /* IDENT  */
    LTLT_OP = 262,                 /* LTLT_OP  */
    GTGT_OP = 263,                 /* GTGT_OP  */
    EQUEQU_OP = 264,               /* EQUEQU_OP  */
    GTEQU_OP = 265,                /* GTEQU_OP  */
    LTEQU_OP = 266,                /* LTEQU_OP  */
    NOTEQU_OP = 267,               /* NOTEQU_OP  */
    ARROW_OP = 268,                /* ARROW_OP  */
    SWAP_OP = 269,                 /* SWAP_OP  */
    ANDAND_OP = 270,               /* ANDAND_OP  */
    OROR_OP = 271,                 /* OROR_OP  */
    SIZEOF_OP = 272,               /* SIZEOF_OP  */
    COLCOL_OP = 273,               /* COLCOL_OP  */
    DOTDOTDOT_OP = 274,            /* DOTDOTDOT_OP  */
    EXTERN_OP = 275,               /* EXTERN_OP  */
    GLOBAL_OP = 276,               /* GLOBAL_OP  */
    VOID_OP = 277,                 /* VOID_OP  */
    BYTE_OP = 278,                 /* BYTE_OP  */
    UBYTE_OP = 279,                /* UBYTE_OP  */
    WORD_OP = 280,                 /* WORD_OP  */
    UWORD_OP = 281,                /* UWORD_OP  */
    LONG_OP = 282,                 /* LONG_OP  */
    ULONG_OP = 283,                /* ULONG_OP  */
    QUAD_OP = 284,                 /* QUAD_OP  */
    UQUAD_OP = 285,                /* UQUAD_OP  */
    SINGLE_OP = 286,               /* SINGLE_OP  */
    DOUBLE_OP = 287,               /* DOUBLE_OP  */
    STRUCT_OP = 288,               /* STRUCT_OP  */
    ENUM_OP = 289,                 /* ENUM_OP  */
    CONST_OP = 290,                /* CONST_OP  */
    SHARED_OP = 291,               /* SHARED_OP  */
    IF_OP = 292,                   /* IF_OP  */
    ELSE_OP = 293,                 /* ELSE_OP  */
    WHILE_OP = 294,                /* WHILE_OP  */
    DO_OP = 295,                   /* DO_OP  */
    FOR_OP = 296,                  /* FOR_OP  */
    BREAK_OP = 297,                /* BREAK_OP  */
    CONTINUE_OP = 298,             /* CONTINUE_OP  */
    SWITCH_OP = 299,               /* SWITCH_OP  */
    CASE_OP = 300,                 /* CASE_OP  */
    DEFAULT_OP = 301,              /* DEFAULT_OP  */
    RETURN_OP = 302,               /* RETURN_OP  */
    GOTO_OP = 303,                 /* GOTO_OP  */
    INCLUDE_OP = 304,              /* INCLUDE_OP  */
    IFDEF_OP = 305,                /* IFDEF_OP  */
    IFNDEF_OP = 306,               /* IFNDEF_OP  */
    THEN_OP = 307,                 /* THEN_OP  */
    DEFINE_OP = 308,               /* DEFINE_OP  */
    UNDEF_OP = 309,                /* UNDEF_OP  */
    NOTHING_OP = 310               /* NOTHING_OP  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
  char* IDENT;                             /* IDENT  */
  double FLOAT;                            /* FLOAT  */
  long long SIGN_NUMBER;                   /* SIGN_NUMBER  */
  unsigned long long NUMBER;               /* NUMBER  */

#line 190 "parserFile.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);



/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUMBER = 3,                     /* NUMBER  */
  YYSYMBOL_SIGN_NUMBER = 4,                /* SIGN_NUMBER  */
  YYSYMBOL_FLOAT = 5,                      /* FLOAT  */
  YYSYMBOL_IDENT = 6,                      /* IDENT  */
  YYSYMBOL_LTLT_OP = 7,                    /* LTLT_OP  */
  YYSYMBOL_GTGT_OP = 8,                    /* GTGT_OP  */
  YYSYMBOL_EQUEQU_OP = 9,                  /* EQUEQU_OP  */
  YYSYMBOL_GTEQU_OP = 10,                  /* GTEQU_OP  */
  YYSYMBOL_LTEQU_OP = 11,                  /* LTEQU_OP  */
  YYSYMBOL_NOTEQU_OP = 12,                 /* NOTEQU_OP  */
  YYSYMBOL_ARROW_OP = 13,                  /* ARROW_OP  */
  YYSYMBOL_SWAP_OP = 14,                   /* SWAP_OP  */
  YYSYMBOL_ANDAND_OP = 15,                 /* ANDAND_OP  */
  YYSYMBOL_OROR_OP = 16,                   /* OROR_OP  */
  YYSYMBOL_SIZEOF_OP = 17,                 /* SIZEOF_OP  */
  YYSYMBOL_COLCOL_OP = 18,                 /* COLCOL_OP  */
  YYSYMBOL_DOTDOTDOT_OP = 19,              /* DOTDOTDOT_OP  */
  YYSYMBOL_EXTERN_OP = 20,                 /* EXTERN_OP  */
  YYSYMBOL_GLOBAL_OP = 21,                 /* GLOBAL_OP  */
  YYSYMBOL_VOID_OP = 22,                   /* VOID_OP  */
  YYSYMBOL_BYTE_OP = 23,                   /* BYTE_OP  */
  YYSYMBOL_UBYTE_OP = 24,                  /* UBYTE_OP  */
  YYSYMBOL_WORD_OP = 25,                   /* WORD_OP  */
  YYSYMBOL_UWORD_OP = 26,                  /* UWORD_OP  */
  YYSYMBOL_LONG_OP = 27,                   /* LONG_OP  */
  YYSYMBOL_ULONG_OP = 28,                  /* ULONG_OP  */
  YYSYMBOL_QUAD_OP = 29,                   /* QUAD_OP  */
  YYSYMBOL_UQUAD_OP = 30,                  /* UQUAD_OP  */
  YYSYMBOL_SINGLE_OP = 31,                 /* SINGLE_OP  */
  YYSYMBOL_DOUBLE_OP = 32,                 /* DOUBLE_OP  */
  YYSYMBOL_STRUCT_OP = 33,                 /* STRUCT_OP  */
  YYSYMBOL_ENUM_OP = 34,                   /* ENUM_OP  */
  YYSYMBOL_CONST_OP = 35,                  /* CONST_OP  */
  YYSYMBOL_SHARED_OP = 36,                 /* SHARED_OP  */
  YYSYMBOL_IF_OP = 37,                     /* IF_OP  */
  YYSYMBOL_ELSE_OP = 38,                   /* ELSE_OP  */
  YYSYMBOL_WHILE_OP = 39,                  /* WHILE_OP  */
  YYSYMBOL_DO_OP = 40,                     /* DO_OP  */
  YYSYMBOL_FOR_OP = 41,                    /* FOR_OP  */
  YYSYMBOL_BREAK_OP = 42,                  /* BREAK_OP  */
  YYSYMBOL_CONTINUE_OP = 43,               /* CONTINUE_OP  */
  YYSYMBOL_SWITCH_OP = 44,                 /* SWITCH_OP  */
  YYSYMBOL_CASE_OP = 45,                   /* CASE_OP  */
  YYSYMBOL_DEFAULT_OP = 46,                /* DEFAULT_OP  */
  YYSYMBOL_RETURN_OP = 47,                 /* RETURN_OP  */
  YYSYMBOL_GOTO_OP = 48,                   /* GOTO_OP  */
  YYSYMBOL_INCLUDE_OP = 49,                /* INCLUDE_OP  */
  YYSYMBOL_IFDEF_OP = 50,                  /* IFDEF_OP  */
  YYSYMBOL_IFNDEF_OP = 51,                 /* IFNDEF_OP  */
  YYSYMBOL_THEN_OP = 52,                   /* THEN_OP  */
  YYSYMBOL_DEFINE_OP = 53,                 /* DEFINE_OP  */
  YYSYMBOL_UNDEF_OP = 54,                  /* UNDEF_OP  */
  YYSYMBOL_NOTHING_OP = 55,                /* NOTHING_OP  */
  YYSYMBOL_56_ = 56,                       /* '('  */
  YYSYMBOL_57_ = 57,                       /* ')'  */
  YYSYMBOL_58_ = 58,                       /* '{'  */
  YYSYMBOL_59_ = 59,                       /* '}'  */
  YYSYMBOL_60_ = 60,                       /* ','  */
  YYSYMBOL_61_ = 61,                       /* '['  */
  YYSYMBOL_62_ = 62,                       /* ']'  */
  YYSYMBOL_63_ = 63,                       /* '<'  */
  YYSYMBOL_64_ = 64,                       /* '>'  */
  YYSYMBOL_65_ = 65,                       /* '.'  */
  YYSYMBOL_66_ = 66,                       /* ':'  */
  YYSYMBOL_67_ = 67,                       /* '~'  */
  YYSYMBOL_68_ = 68,                       /* '!'  */
  YYSYMBOL_69_ = 69,                       /* '+'  */
  YYSYMBOL_70_ = 70,                       /* '-'  */
  YYSYMBOL_71_ = 71,                       /* '&'  */
  YYSYMBOL_72_ = 72,                       /* '*'  */
  YYSYMBOL_73_ = 73,                       /* '|'  */
  YYSYMBOL_74_ = 74,                       /* '/'  */
  YYSYMBOL_75_ = 75,                       /* '%'  */
  YYSYMBOL_76_ = 76,                       /* '^'  */
  YYSYMBOL_77_ = 77,                       /* '?'  */
  YYSYMBOL_78_ = 78,                       /* '='  */
  YYSYMBOL_79_ = 79,                       /* ';'  */
  YYSYMBOL_80_ = 80,                       /* '"'  */
  YYSYMBOL_81_ = 81,                       /* '#'  */
  YYSYMBOL_YYACCEPT = 82,                  /* $accept  */
  YYSYMBOL_input = 83,                     /* input  */
  YYSYMBOL_constant = 84,                  /* constant  */
  YYSYMBOL_initial_expression = 85,        /* initial_expression  */
  YYSYMBOL_initializer = 86,               /* initializer  */
  YYSYMBOL_initializer_list = 87,          /* initializer_list  */
  YYSYMBOL_iota_vector = 88,               /* iota_vector  */
  YYSYMBOL_postfix_operation = 89,         /* postfix_operation  */
  YYSYMBOL_permute_list = 90,              /* permute_list  */
  YYSYMBOL_block_permute = 91,             /* block_permute  */
  YYSYMBOL_argument_list = 92,             /* argument_list  */
  YYSYMBOL_prefix_operation = 93,          /* prefix_operation  */
  YYSYMBOL_multdiv_operation = 94,         /* multdiv_operation  */
  YYSYMBOL_addsub_operation = 95,          /* addsub_operation  */
  YYSYMBOL_shift_operation = 96,           /* shift_operation  */
  YYSYMBOL_relation_operation = 97,        /* relation_operation  */
  YYSYMBOL_equality_operation = 98,        /* equality_operation  */
  YYSYMBOL_bitwise_and_operation = 99,     /* bitwise_and_operation  */
  YYSYMBOL_bitwise_eor_operation = 100,    /* bitwise_eor_operation  */
  YYSYMBOL_bitwise_or_operation = 101,     /* bitwise_or_operation  */
  YYSYMBOL_mesh_operation = 102,           /* mesh_operation  */
  YYSYMBOL_logical_and_operation = 103,    /* logical_and_operation  */
  YYSYMBOL_logical_or_operation = 104,     /* logical_or_operation  */
  YYSYMBOL_ternary_operation = 105,        /* ternary_operation  */
  YYSYMBOL_assignment_operation = 106,     /* assignment_operation  */
  YYSYMBOL_comma_operation = 107,          /* comma_operation  */
  YYSYMBOL_expression = 108,               /* expression  */
  YYSYMBOL_base_type = 109,                /* base_type  */
  YYSYMBOL_struct_declaration_list = 110,  /* struct_declaration_list  */
  YYSYMBOL_struct_declaration = 111,       /* struct_declaration  */
  YYSYMBOL_struct_declaration_initializer_list = 112, /* struct_declaration_initializer_list  */
  YYSYMBOL_enumerator_list = 113,          /* enumerator_list  */
  YYSYMBOL_base_type_postfix = 114,        /* base_type_postfix  */
  YYSYMBOL_array_modifier = 115,           /* array_modifier  */
  YYSYMBOL_pointer_modifier = 116,         /* pointer_modifier  */
  YYSYMBOL_function_modifier = 117,        /* function_modifier  */
  YYSYMBOL_type_name = 118,                /* type_name  */
  YYSYMBOL_parameter_list = 119            /* parameter_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   448

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  82
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  157
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  288

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   310


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    68,    80,    81,     2,    75,    71,     2,
      56,    57,    72,    69,    60,    70,    65,    74,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    66,    79,
      63,    78,    64,    77,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    61,     2,    62,    76,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    58,    73,    59,    67,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    78,    78,    79,    83,    84,    85,    89,    90,    91,
      92,    93,    94,    98,    99,   103,   104,   108,   109,   110,
     111,   112,   113,   114,   115,   119,   120,   121,   122,   123,
     124,   125,   126,   130,   131,   132,   133,   137,   138,   142,
     143,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   162,   163,   164,   165,   169,   170,   171,
     175,   176,   177,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   193,   194,   195,   196,   197,   201,   202,   206,
     207,   211,   212,   216,   217,   221,   222,   226,   227,   231,
     232,   236,   237,   238,   242,   243,   247,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   279,   280,   281,   282,
     283,   287,   288,   292,   296,   297,   301,   302,   303,   304,
     308,   309,   310,   311,   312,   313,   317,   318,   319,   320,
     321,   322,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   341,   342,   343,   344,   345,   346,
     350,   351,   352,   353,   358,   359,   360,   361
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUMBER",
  "SIGN_NUMBER", "FLOAT", "IDENT", "LTLT_OP", "GTGT_OP", "EQUEQU_OP",
  "GTEQU_OP", "LTEQU_OP", "NOTEQU_OP", "ARROW_OP", "SWAP_OP", "ANDAND_OP",
  "OROR_OP", "SIZEOF_OP", "COLCOL_OP", "DOTDOTDOT_OP", "EXTERN_OP",
  "GLOBAL_OP", "VOID_OP", "BYTE_OP", "UBYTE_OP", "WORD_OP", "UWORD_OP",
  "LONG_OP", "ULONG_OP", "QUAD_OP", "UQUAD_OP", "SINGLE_OP", "DOUBLE_OP",
  "STRUCT_OP", "ENUM_OP", "CONST_OP", "SHARED_OP", "IF_OP", "ELSE_OP",
  "WHILE_OP", "DO_OP", "FOR_OP", "BREAK_OP", "CONTINUE_OP", "SWITCH_OP",
  "CASE_OP", "DEFAULT_OP", "RETURN_OP", "GOTO_OP", "INCLUDE_OP",
  "IFDEF_OP", "IFNDEF_OP", "THEN_OP", "DEFINE_OP", "UNDEF_OP",
  "NOTHING_OP", "'('", "')'", "'{'", "'}'", "','", "'['", "']'", "'<'",
  "'>'", "'.'", "':'", "'~'", "'!'", "'+'", "'-'", "'&'", "'*'", "'|'",
  "'/'", "'%'", "'^'", "'?'", "'='", "';'", "'\"'", "'#'", "$accept",
  "input", "constant", "initial_expression", "initializer",
  "initializer_list", "iota_vector", "postfix_operation", "permute_list",
  "block_permute", "argument_list", "prefix_operation",
  "multdiv_operation", "addsub_operation", "shift_operation",
  "relation_operation", "equality_operation", "bitwise_and_operation",
  "bitwise_eor_operation", "bitwise_or_operation", "mesh_operation",
  "logical_and_operation", "logical_or_operation", "ternary_operation",
  "assignment_operation", "comma_operation", "expression", "base_type",
  "struct_declaration_list", "struct_declaration",
  "struct_declaration_initializer_list", "enumerator_list",
  "base_type_postfix", "array_modifier", "pointer_modifier",
  "function_modifier", "type_name", "parameter_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-80)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-38)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -80,    10,   -80,   -80,   -80,   -80,   -80,   -80,   293,    56,
     293,   -80,   -80,   -80,   -80,   293,   -38,   -80,   -80,   -80,
     -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,    42,    46,
     414,   414,   293,   293,   293,   293,   293,   293,   293,   293,
     -80,   104,    -9,    89,   107,   172,    15,    85,    12,    34,
      62,     6,   151,   -13,   -80,   -80,   123,    59,   139,     2,
       0,    40,    90,   143,   -80,   -80,   129,   146,   -80,   399,
     149,   158,   159,   161,   135,   -80,   -80,   -80,   -80,   -80,
     -80,   -80,   203,   222,   293,    37,   215,   293,   293,   293,
     293,   293,   293,   293,   293,   293,   293,   293,   293,   293,
     293,   293,   293,   293,   293,   293,   293,   293,   293,   293,
     -80,   293,   182,   -55,   -33,   311,   293,   -80,   157,   160,
     -80,   -17,   -10,   348,   293,   -80,   162,   163,   -80,   293,
     -80,    56,   293,   176,   399,   225,   293,   293,   293,   -80,
     -80,   111,   -80,   178,   224,   -80,   -15,     1,   -80,   -80,
     -80,   -80,   -80,   -80,   -80,    89,    89,   107,   107,     4,
      14,    16,    26,    36,    43,    85,    12,    34,    62,     6,
     151,   177,   -80,    58,   195,   293,   -80,   293,   -80,   204,
     254,   113,   200,   -80,   -80,   293,   -80,   293,   -80,   206,
     118,   202,   -80,   -80,   208,   -80,    70,   -80,   134,   -80,
     260,   189,   136,   205,   210,   -80,   -80,   293,   -80,    37,
     -80,   182,   -80,   293,   293,   293,   293,   293,   293,   293,
     -80,   293,   -80,   -80,   214,   219,   -80,   -80,   -80,   366,
     -80,   220,   226,   -80,   -80,   384,   -80,   -80,   -80,   293,
     -80,   -80,   399,   -80,   -29,   182,   -80,   262,   -80,   -80,
     -80,   211,   -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,
     -30,   -80,   -80,   213,   266,   -80,   -80,   227,    74,   -80,
     280,   -80,   -80,   209,    37,   -80,   -80,   -80,   -80,   -80,
     -80,   -80,   -80,   182,   234,   -80,    37,   -80
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     5,     4,     6,     7,     9,     0,     0,
       0,     8,     3,    12,    11,     0,     0,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      25,    41,    53,    57,    60,    63,    72,    77,    79,    81,
      83,    85,    87,    89,    91,    94,    96,     0,   120,   150,
     152,   153,   151,     0,    13,    15,     0,     0,    49,     0,
     108,     0,   122,   123,     0,    42,    43,    44,    45,    46,
      47,    48,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,   132,     0,     0,
     134,     0,     0,     0,     0,   133,     0,     0,   135,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,    32,
      27,     0,    39,     0,     8,    33,     0,     0,    31,    93,
      92,    54,    55,    56,    53,    58,    59,    62,    61,    67,
      66,    64,    65,    73,    74,    78,    80,    82,    84,    86,
      88,     0,    95,     0,     0,     0,   136,     0,   140,     0,
     154,     0,     0,   138,   142,     0,   137,     0,   141,     0,
       0,     0,   139,   143,     0,    16,     0,    52,     0,   111,
       0,   116,     0,     0,     0,    50,    28,     0,    26,     0,
      29,     0,    30,     0,     0,     0,     0,     0,     0,     0,
      20,     0,    18,   121,     0,     0,   148,   155,   144,     0,
     126,     0,     0,   149,   145,     0,   127,    51,    19,     0,
      17,   109,     0,   114,     0,     0,   110,     0,   124,   125,
      40,    34,    38,    71,    70,    68,    69,    75,    76,    90,
       0,   128,   129,     0,   156,   130,   131,     0,     0,   112,
       0,   113,   117,   118,     0,    24,    22,   146,   157,   147,
      23,    21,   115,     0,    35,   119,     0,    36
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -80,   -80,    -8,    -1,    -7,   -80,   -80,   -80,   -80,   -80,
     -80,    52,   105,   106,    41,   119,   199,   201,   198,   243,
     197,   207,   -80,    86,   -79,   -80,    -3,   174,   -80,    64,
     -80,   -80,   -80,   -80,   -80,   -80,   -12,   184
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,    11,    40,    13,    66,    14,    41,   146,   147,
     141,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,   198,   199,
     244,   202,    59,    60,    61,    62,    63,   181
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      12,    64,    65,   107,   142,    87,   175,    67,   149,   150,
       2,    94,    95,     3,     4,     5,     6,   176,    69,   211,
      74,    94,    95,    94,    95,    96,    97,   275,   177,     7,
     172,   270,   276,    94,    95,   118,   119,   113,   114,   178,
       3,     4,     5,     6,   185,   209,    96,    97,    70,   210,
     271,   187,    71,    96,    97,   186,     7,   133,   115,     3,
       4,     5,   188,   116,   108,   212,     8,    68,     9,    88,
     213,    10,   120,   105,   117,   121,   122,   144,    98,    99,
     214,   143,   215,   102,   145,    75,    76,    77,    78,    79,
      80,    81,   216,     8,   100,     9,   123,   101,    10,    98,
      99,   124,   217,   180,   174,   171,    98,    99,   173,   218,
     103,   180,   125,   182,     9,   220,   110,    82,   221,   111,
     222,   191,   200,    64,   195,   126,   127,   238,   250,   196,
     239,   280,   240,   203,   204,   104,   281,   159,   160,   161,
     162,   151,   152,   153,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
      83,    89,   128,    90,    91,    84,   106,    85,   206,    86,
     228,   207,   224,   229,   225,   234,    92,    93,   235,    94,
      95,   194,   231,   109,   232,     3,     4,     5,   130,   131,
     205,   129,   138,   241,   242,   246,   247,   155,   156,   129,
     157,   158,   112,   252,    72,    73,   132,   134,   251,   139,
     253,   254,   255,   256,   257,   258,   135,   264,   260,   163,
     164,   148,   136,   264,   137,     3,     4,     5,     6,   183,
     200,   201,   184,   197,   192,   193,   268,   272,    15,    16,
     208,     7,   -37,   219,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,   223,
     227,   226,   230,   233,   236,   237,   243,   245,   273,   248,
     277,   154,   278,   284,   249,   285,   261,   274,    32,   140,
       9,   262,   265,    10,   279,   287,   282,   283,   266,    33,
      34,    35,    36,    37,    38,    39,     3,     4,     5,     6,
     286,   165,   167,   169,   166,   259,   269,   190,     0,    15,
      16,     0,     7,     0,   170,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
     179,     0,     0,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,   168,    32,
       0,     9,     0,     0,    10,     0,     0,     0,     0,     0,
      33,    34,    35,    36,    37,    38,    39,   189,     0,     0,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,   263,     0,     0,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,   267,     0,     0,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29
};

static const yytype_int16 yycheck[] =
{
       1,     9,     9,    16,    83,    14,    61,    10,    87,    88,
       0,     7,     8,     3,     4,     5,     6,    72,    56,    18,
      32,     7,     8,     7,     8,    10,    11,    57,    61,    19,
     109,    60,    62,     7,     8,    35,    36,    35,    36,    72,
       3,     4,     5,     6,    61,    60,    10,    11,     6,    64,
      79,    61,     6,    10,    11,    72,    19,    69,    56,     3,
       4,     5,    72,    61,    77,    64,    56,    15,    58,    78,
      66,    61,    72,    67,    72,    35,    36,    85,    63,    64,
      66,    84,    66,    71,    85,    33,    34,    35,    36,    37,
      38,    39,    66,    56,     9,    58,    56,    12,    61,    63,
      64,    61,    66,   115,   112,   108,    63,    64,   111,    66,
      76,   123,    72,   116,    58,    57,    57,    13,    60,    60,
      62,   124,   134,   131,   131,    35,    36,    57,   207,   132,
      60,    57,    62,   136,   137,    73,    62,    96,    97,    98,
      99,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
      56,    72,    72,    74,    75,    61,    15,    63,    57,    65,
      57,    60,   175,    60,   177,    57,    69,    70,    60,     7,
       8,   129,   185,    60,   187,     3,     4,     5,    59,    60,
     138,    56,    57,    59,    60,    59,    60,    92,    93,    56,
      94,    95,    63,   211,    30,    31,    60,    58,   209,     6,
     213,   214,   215,   216,   217,   218,    58,   229,   221,   100,
     101,     6,    63,   235,    63,     3,     4,     5,     6,    72,
     242,     6,    72,    57,    72,    72,   239,   245,    16,    17,
      62,    19,    18,    66,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    64,
       6,    57,    62,    57,    62,    57,     6,    78,     6,    64,
      57,   219,     6,   274,    64,   283,    62,    66,    56,    57,
      58,    62,    62,    61,    57,   286,     6,    78,    62,    67,
      68,    69,    70,    71,    72,    73,     3,     4,     5,     6,
      66,   102,   104,   106,   103,   219,   242,   123,    -1,    16,
      17,    -1,    19,    -1,   107,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      19,    -1,    -1,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,   105,    56,
      -1,    58,    -1,    -1,    61,    -1,    -1,    -1,    -1,    -1,
      67,    68,    69,    70,    71,    72,    73,    19,    -1,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    19,    -1,    -1,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    19,    -1,    -1,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    83,     0,     3,     4,     5,     6,    19,    56,    58,
      61,    84,    85,    86,    88,    16,    17,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    56,    67,    68,    69,    70,    71,    72,    73,
      85,    89,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   114,
     115,   116,   117,   118,    84,    86,    87,   108,    93,    56,
       6,     6,   109,   109,   118,    93,    93,    93,    93,    93,
      93,    93,    13,    56,    61,    63,    65,    14,    78,    72,
      74,    75,    69,    70,     7,     8,    10,    11,    63,    64,
       9,    12,    71,    76,    73,    67,    15,    16,    77,    60,
      57,    60,    63,    35,    36,    56,    61,    72,    35,    36,
      72,    35,    36,    56,    61,    72,    35,    36,    72,    56,
      59,    60,    60,   118,    58,    58,    63,    63,    57,     6,
      57,    92,   106,   108,    84,    85,    90,    91,     6,   106,
     106,    93,    93,    93,    93,    94,    94,    95,    95,    96,
      96,    96,    96,    97,    97,    98,    99,   100,   101,   102,
     103,   108,   106,   108,    84,    61,    72,    61,    72,    19,
     118,   119,   108,    72,    72,    61,    72,    61,    72,    19,
     119,   108,    72,    72,    93,    86,   108,    57,   110,   111,
     118,     6,   113,   108,   108,    93,    57,    60,    62,    60,
      64,    18,    64,    66,    66,    66,    66,    66,    66,    66,
      57,    60,    62,    64,   108,   108,    57,     6,    57,    60,
      62,   108,   108,    57,    57,    60,    62,    57,    57,    60,
      62,    59,    60,     6,   112,    78,    59,    60,    64,    64,
     106,    85,    84,   108,   108,   108,   108,   108,   108,   105,
     108,    62,    62,    19,   118,    62,    62,    19,   108,   111,
      60,    79,    84,     6,    66,    57,    62,    57,     6,    57,
      57,    62,     6,    78,    85,    84,    66,    85
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    82,    83,    83,    84,    84,    84,    85,    85,    85,
      85,    85,    85,    86,    86,    87,    87,    88,    88,    88,
      88,    88,    88,    88,    88,    89,    89,    89,    89,    89,
      89,    89,    89,    90,    90,    90,    90,    91,    91,    92,
      92,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    93,    93,    94,    94,    94,    94,    95,    95,    95,
      96,    96,    96,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    98,    98,    98,    98,    98,    99,    99,   100,
     100,   101,   101,   102,   102,   103,   103,   104,   104,   105,
     105,   106,   106,   106,   107,   107,   108,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   110,   110,   111,   112,   112,   113,   113,   113,   113,
     114,   114,   114,   114,   114,   114,   115,   115,   115,   115,
     115,   115,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   117,   117,   117,   117,   117,   117,
     118,   118,   118,   118,   119,   119,   119,   119
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     3,     1,     3,     5,     5,     5,
       5,     7,     7,     7,     7,     1,     4,     3,     4,     4,
       4,     3,     3,     1,     3,     5,     7,     1,     3,     1,
       3,     1,     2,     2,     2,     2,     2,     2,     2,     2,
       4,     4,     4,     1,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     3,     5,     5,
       5,     5,     1,     3,     3,     5,     5,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       5,     1,     3,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     5,
       5,     1,     3,     3,     1,     3,     1,     3,     3,     5,
       1,     4,     2,     2,     5,     5,     4,     4,     5,     5,
       5,     5,     2,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     4,     4,     6,     6,     4,     4,
       1,     1,     1,     1,     1,     2,     3,     4
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 4: /* constant: SIGN_NUMBER  */
#line 83 "parserFile.y"
                        {printf("%lld\n", (yyvsp[0].SIGN_NUMBER));}
#line 1526 "parserFile.tab.c"
    break;

  case 5: /* constant: NUMBER  */
#line 84 "parserFile.y"
                        {printf("%llu\n", (yyvsp[0].NUMBER));}
#line 1532 "parserFile.tab.c"
    break;

  case 6: /* constant: FLOAT  */
#line 85 "parserFile.y"
                        {printf("%lf\n", (yyvsp[0].FLOAT));}
#line 1538 "parserFile.tab.c"
    break;


#line 1542 "parserFile.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 421 "parserFile.y"


#include "lex.yy.c"

void yyerror(char const *s){
	fprintf(stderr, "%s\n", s);
}

int main(int argc, char *argv[]){
	inFile = stdin;
	outFile = stdout;

	yyin = stdin;
	yyparse();

	//while(/*files not empty*/){
		//look for delimiters
	//	char *cut;
	//
	//	struct yy_buffer_state *selection = yy_scan_string(cut);
	//	yyparse();
	//	yy_delete_buffer(selection);
	//}
	

	return 0;
}



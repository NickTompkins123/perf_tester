/**
* \file
* Settings parser interface
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_settings_parser.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/string/kzc_string.h>
#include <core/kzc_error_codes.h>

#include <system/wrappers/kzs_string.h>
#include <system/wrappers/kzs_memory.h>


/* TODO: Improve parser to explain parsing errors better. print erroneous line/token and row number */
/* TODO: Add escaped characters, " etc */
/* TODO: Invalidate string tokens as names */

#define KZC_SETTINGS_OPERATOR_SECTION_START '{'
#define KZC_SETTINGS_OPERATOR_SECTION_END '}'
#define KZC_SETTINGS_OPERATOR_ASSIGN '='

#define KZC_SETTINGS_START_COMMENT '#'


/* TokenType possible token types. Values chosen so they can be stored in a bit field. */
#define KZC_SETTINGS_SECTION_START_MASK 1
#define KZC_SETTINGS_SECTION_END_MASK 2
#define KZC_SETTINGS_ASSIGN_MASK 4
#define KZC_SETTINGS_STRING_MASK 8


typedef kzUint TokenType;


/** Container for a single token. */
struct SettingParseToken
{
    kzUint start; /**< Start index of originating string in reference buffer. */
    kzUint end;   /**< Last index + 1 of originating string in reference buffer. */
    kzUint line;  /**< Line number of token. */
    kzString source; /**< Buffer where token string data lives. */
    kzBool isString; /**< Is the tokenized string read in as an explicit string. */
};


/** Return true if the character is an operator symbol. */
static kzBool kzcSettingsIsCharOperator_internal(kzChar c)
{
    return c == KZC_SETTINGS_OPERATOR_SECTION_START || c == KZC_SETTINGS_OPERATOR_SECTION_END || c == KZC_SETTINGS_OPERATOR_ASSIGN;
}

/** Return type of token. */
static TokenType kzcSettingsGetTokenType_internal(const struct SettingParseToken* token)
{
    TokenType tokenType = KZC_SETTINGS_STRING_MASK;

    if(kzcSettingsIsCharOperator_internal(token->source[token->start]))
    {
        kzChar operatorChar = token->source[token->start];
        switch(operatorChar)
        {
            case KZC_SETTINGS_OPERATOR_ASSIGN:
            {
                tokenType = KZC_SETTINGS_ASSIGN_MASK;
                break;
            }
            case KZC_SETTINGS_OPERATOR_SECTION_START:
            {
                tokenType = KZC_SETTINGS_SECTION_START_MASK;
                break;
            }
            case KZC_SETTINGS_OPERATOR_SECTION_END:
            {
                tokenType = KZC_SETTINGS_SECTION_END_MASK;
                break;
            }
            default:
            {
                kzsAssert(KZ_FALSE);
                break;
            }
        }
    }
    return tokenType;
}

/** Delete tokens from array and the array itself. */
static kzsError kzcSettingsDeleteTokensArray_internal(struct KzcDynamicArray* tokens)
{
    kzsError result;
    /* TODO: no need for MutableIterator */
    struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(tokens);
    while(kzcDynamicArrayIterate(it))
    {
        struct SettingParseToken* token = (struct SettingParseToken*)kzcDynamicArrayIteratorGetValue(it);
        result = kzcMemoryFreeVariable(token);
        kzsErrorForward(result);
    }
    result = kzcDynamicArrayDelete(tokens);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Insert a token to a dynamic array */
static kzsError kzcSettingsPushToken_internal(const struct KzcMemoryManager* memoryManager, struct KzcDynamicArray* tokens,
                                              kzString source, kzUint start, kzUint end, kzUint line, kzBool isString)
{
    kzsError result;

    struct SettingParseToken* token;
    result = kzcMemoryAllocVariable(memoryManager, token, "SettingParseToken");
    kzsErrorForward(result);

    token->start = start;
    token->end = end;
    token->line = line;
    token->source = source;
    token->isString = isString;

    result = kzcDynamicArrayAdd(tokens, token);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Populate array with tokens. */
static kzsError kzcSettingsParserTokenize_internal(const struct KzcMemoryManager* memoryManager, kzString source,
                                                   const kzUint sourceLength, struct KzcDynamicArray* tokens)
{
    kzsError result;
    kzUint i = 0;

    kzUint tokenStart = 0;
    kzUint tokenEnd = 0;
    kzUint line = 0;

    kzBool notReadingString = KZ_TRUE;  /* TODO: change to isReadingString */
    const kzChar stringDelimiter = '"'; /* TODO: use #define */

    /* Walk through the source buffer */
    while(i < sourceLength)
    {
        kzChar si = source[i];

        /* Handle comment */
        if(si == KZC_SETTINGS_START_COMMENT && notReadingString)
        {
            /* If there is a token in the string behind, push it*/
            if(tokenStart != tokenEnd)
            {
                /* Push previous token */
                result = kzcSettingsPushToken_internal(memoryManager, tokens, source, tokenStart, tokenEnd, line, KZ_FALSE);
                kzsErrorForward(result);
            }
            /* Walk until end of line */
            while(i < sourceLength && si != '\n')    /* TODO: use #define for '\n' */
            {
                ++i;
                si = source[i];
            }
            tokenStart = i + 1;
            tokenEnd = tokenStart;
        }
        /* Is current char whitespace */
        else if(kzsIsSpace(si) && notReadingString)  /* TODO: check handling of \r */
        {
            /* TODO: improve comments */
            /* If current is whitespace and parsing symbol, end parsing and start next symbol
               If not parsing, just forward to next */
            /* If there is a token in the string behind, push it*/

            if(tokenStart != tokenEnd)
            {
                /* Push previous token */
                result = kzcSettingsPushToken_internal(memoryManager, tokens, source, tokenStart, tokenEnd, line, KZ_FALSE);
                kzsErrorForward(result);
            }
            tokenStart = i + 1;
            tokenEnd = tokenStart;
        }
        /* Is current char an operator '=','{' or '}' */
        else if(kzcSettingsIsCharOperator_internal(si) && notReadingString)
        {
            /* If there is a token in the string behind*/
            if(tokenStart != tokenEnd)
            {
                /* Push previous token */
                result = kzcSettingsPushToken_internal(memoryManager, tokens, source, tokenStart, tokenEnd, line, KZ_FALSE);
                kzsErrorForward(result);
            }
            /* Push operator token */
            result = kzcSettingsPushToken_internal(memoryManager, tokens, source, i, i + 1, line, KZ_FALSE);
            kzsErrorForward(result);
            tokenStart = i + 1;
            tokenEnd = tokenStart;
        }
        /* Is current char string delimiter? */
        else if(si == stringDelimiter)
        {
            /* Start reading string */
            if(notReadingString)
            {
                notReadingString = KZ_FALSE;
            }
            /* Stop reading string */
            else
            {
                /* Push string as token. Note: do not push the limiting "" markers */
                notReadingString = KZ_TRUE;
                result = kzcSettingsPushToken_internal(memoryManager, tokens, source, tokenStart, tokenEnd, line, KZ_TRUE);
                kzsErrorForward(result);
            }
            tokenStart = i + 1;
            tokenEnd = tokenStart;
        }
        /* Not a breaking symbol - just forward last index to next character*/
        else
        {
            ++tokenEnd;
        }

        ++i;
    }

    /* Handle last token, if any. */
    if(tokenStart != tokenEnd)
    {
        /* Push previous token */
        result = kzcSettingsPushToken_internal(memoryManager, tokens, source, tokenStart, tokenEnd, line, !notReadingString);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

static kzsError kzcSettingsParserCopyTokenToString_internal(const struct SettingParseToken* token, kzMutableString* out_string)
{
    kzsError result;
    struct KzcMemoryManager* manager;
    kzUint length = token->end - token->start;
    kzMutableString string;

    kzsAssert(kzcIsValidPointer(token));

    manager = kzcMemoryGetManager(token);

    result = kzcMemoryAllocArray(manager, string, length + 1, "string");
    kzsErrorForward(result);

    kzsMemcpy(string, token->source + token->start, length);
    string[length] = KZC_STRING_TERMINATION_SYMBOL;

    *out_string = string;

    kzsSuccess();
}

/* Add new setting to node. Setting may be integer|float|string */
static kzsError kzcSettingsNewSettingFromTokens_internal(const struct KzcSettingNode* node, const struct SettingParseToken* name,
                                              const struct SettingParseToken* value)
{
    kzsError result;
    kzMutableString nameBuffer;
    kzMutableString valueBuffer;
    kzUint i;

    result = kzcSettingsParserCopyTokenToString_internal(name, &nameBuffer);
    kzsErrorForward(result);

    result = kzcSettingsParserCopyTokenToString_internal(value, &valueBuffer);
    kzsErrorForward(result);

    if(value->isString)
    {
        /* Add string. No additional parsing needed. */
        result = kzcSettingNodeAddString(node, nameBuffer, valueBuffer);
        kzsErrorForward(result);
    }
    else
    {
        /* Add a number. Integer or float.*/
        kzUint valueLength = value->end - value->start;
        kzBool isInteger = KZ_TRUE;
        /* If the number does not contain any other characters than numbers, it is a float */

        /* Check for ',', '.', 'e' etc */
        for(i = 0; i < valueLength; ++i)
        {
            kzChar c = valueBuffer[i];

            if(i == 0 && (c == '+' || c == '-'))
            {
                continue;
            }
            else if(!kzsIsDigit(c))
            {
                isInteger = KZ_FALSE;
                break;
            }
        }

        if(isInteger)
        {
            kzInt intNumber = kzcStringToInt(valueBuffer);
            result = kzcSettingNodeAddInteger(node, nameBuffer, intNumber);
            kzsErrorForward(result);
        }
        else
        {
            kzFloat floatNumber = kzcStringToFloat(valueBuffer);
            result = kzcSettingNodeAddFloat(node, nameBuffer, floatNumber);
            kzsErrorForward(result);
        }
    }

    result = kzcMemoryFreeArray(nameBuffer);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray(valueBuffer);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Add new subnode to node. Set current node to the new node. */
static kzsError kzcSettingsNewSettingNode_internal(const struct KzcSettingNode* node, const struct SettingParseToken* name,
                                        struct KzcSettingNode** out_newNode)
{
    kzsError result;
    kzMutableString nameString;
    struct KzcSettingNode* newNode;

    result = kzcSettingsParserCopyTokenToString_internal(name, &nameString);
    kzsErrorForward(result);

    result = kzcSettingNodeAddSubNode(node, nameString, &newNode);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray(nameString);
    kzsErrorForward(result);

    *out_newNode = newNode;
    kzsSuccess();
}

/** Check that the token array is valid. */
static kzsError kzcSettingsCheckTokenArrayCorrectness_internal(const struct KzcDynamicArray* tokens)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    kzUint expectedTokens = KZC_SETTINGS_STRING_MASK;

    kzInt treeDepth = 0;

    struct SettingParseToken* nameToken = KZ_NULL;

    /* Walk through the token array */
    it = kzcDynamicArrayGetIterator(tokens);
    while(kzcDynamicArrayIterate(it))
    {
        struct SettingParseToken* token = (struct SettingParseToken*)kzcDynamicArrayIteratorGetValue(it);
        TokenType tokenTypeBit = kzcSettingsGetTokenType_internal(token);
        /* Check if it is a legal token */
        if((tokenTypeBit & expectedTokens) == 0)
        {
            /* Break parsing if wrong type of token found */
            kzsErrorThrow(KZC_ERROR_PARSER_UNEXPECTED_TOKEN, "Unexpected token type");
        }

        /* Handle legal token */
        switch(tokenTypeBit)
        {
            case KZC_SETTINGS_STRING_MASK:
            {
                if(nameToken == KZ_NULL)
                {
                    /* Found name for next setting */
                    nameToken = token;
                    expectedTokens = KZC_SETTINGS_ASSIGN_MASK;
                }
                else
                {
                    /* Found value for next setting, clear value token */
                    nameToken = KZ_NULL;
                    /* Next we expect to find a name */
                    expectedTokens = KZC_SETTINGS_STRING_MASK;
                    /* A section end is also a legal token if we are not at root*/
                    if(treeDepth > 0)
                    {
                        expectedTokens |= KZC_SETTINGS_SECTION_END_MASK;
                    }
                }
                break;
            }
            case KZC_SETTINGS_ASSIGN_MASK:
            {
                expectedTokens = KZC_SETTINGS_SECTION_START_MASK | KZC_SETTINGS_STRING_MASK;
                break;
            }
            case KZC_SETTINGS_SECTION_START_MASK:
            {
                /* Push new node */
                ++treeDepth;
                /* Next should be a name or this is an empty node */
                nameToken = KZ_NULL;
                expectedTokens = KZC_SETTINGS_STRING_MASK | KZC_SETTINGS_SECTION_END_MASK;
                break;
            }
            case KZC_SETTINGS_SECTION_END_MASK:
            {
                --treeDepth;
                kzsErrorTest(treeDepth >= 0, KZC_ERROR_PARSER_PAIRLESS_BRACE, "Brace mismatch");
                expectedTokens = KZC_SETTINGS_STRING_MASK;
                /* A section end is also a legal token if we are not at root*/
                if(treeDepth > 0)
                {
                    expectedTokens |= KZC_SETTINGS_SECTION_END_MASK;
                }
                break;
            }
            default:
            {
                kzsAssert(KZ_FALSE);
                break;
            }
        }
    }

    if(treeDepth > 0)
    {
        result = KZC_ERROR_PARSER_UNCLOSED_SECTION;
        kzsErrorForward(result);
    }

    kzsSuccess();
}

/* Parse tokens to settings tree */
static kzsError kzcSettingsParse_internal(const struct KzcDynamicArray* tokens, const struct KzcSettingContainer* settings,
                                          struct KzcDynamicArray* nodeStack)
{
    kzsError result;

    struct KzcSettingNode* currentNode;
    struct KzcSettingNode* rootNode;

    struct KzcDynamicArrayIterator it;

    struct SettingParseToken* nameToken = KZ_NULL;

    /* First check that the token array is legal */
    result = kzcSettingsCheckTokenArrayCorrectness_internal(tokens);
    kzsErrorForward(result);

    /* Init nodes */
    rootNode = kzcSettingContainerGetRoot(settings);
    currentNode = rootNode;

    /* Walk through the token array */
    it = kzcDynamicArrayGetIterator(tokens);
    while(kzcDynamicArrayIterate(it))
    {
        /* The legality of the token array is verified beforehand. Just
         walk through the array and allocate settings from tokens. */

        struct SettingParseToken* token = (struct SettingParseToken*)kzcDynamicArrayIteratorGetValue(it);
        TokenType tokenType = kzcSettingsGetTokenType_internal(token);

        /* Handle token */
        switch(tokenType)
        {
            case KZC_SETTINGS_STRING_MASK:
            {
                if(nameToken == KZ_NULL)
                {
                    /* Found name for next setting */
                    nameToken = token;
                }
                else
                {
                    /* Found value for next setting -> Push setting*/
                    result = kzcSettingsNewSettingFromTokens_internal(currentNode, nameToken, token);
                    kzsErrorForward(result);
                    nameToken = KZ_NULL;
                }
                break;
            }
            case KZC_SETTINGS_ASSIGN_MASK:
            {
                /* Nothing to do here */
                break;
            }
            case KZC_SETTINGS_SECTION_START_MASK:
            {
                /* Push new node */
                struct KzcSettingNode* newNode;
                result = kzcSettingsNewSettingNode_internal(currentNode, nameToken, &newNode);
                kzsErrorForward(result);
                nameToken = KZ_NULL;
                /* Push current node to top of node stack */
                result = kzcDynamicArrayAdd(nodeStack, currentNode);
                kzsErrorForward(result);
                currentNode = newNode;
                break;
            }
            case KZC_SETTINGS_SECTION_END_MASK:
            {
                kzUint topIndex = kzcDynamicArrayGetSize(nodeStack) - 1;
                currentNode = kzcDynamicArrayGet(nodeStack, topIndex);
                result = kzcDynamicArrayRemoveFromIndex(nodeStack, topIndex);
                kzsErrorForward(result);
                break;
            }
            default:
            {
                kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Bad token value");
            }
        }
    }

    kzsSuccess();
}

kzsError kzcSettingsParseString(const struct KzcSettingContainer* settings, kzString source)
{
    kzsError result;
    struct KzcDynamicArray* tokens;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(settings);
    kzUint sourceLength = kzcStringLength(source);
    struct KzcDynamicArray* nodeStack;

    /* Init data */
    result = kzcDynamicArrayCreate(memoryManager, &tokens);
    kzsErrorForward(result);
    result = kzcDynamicArrayCreate(memoryManager, &nodeStack);
    kzsErrorForward(result);

    /* Tokenize*/
    result = kzcSettingsParserTokenize_internal(memoryManager, source, sourceLength, tokens);
    kzsErrorForward(result);

    /* Parse token string to options */
    result = kzcSettingsParse_internal(tokens, settings, nodeStack);
    kzsErrorForward(result);

    result= kzcDynamicArrayDelete(nodeStack);
    kzsErrorForward(result);
    result = kzcSettingsDeleteTokensArray_internal(tokens);
    kzsErrorForward(result);

    kzsSuccess();
}

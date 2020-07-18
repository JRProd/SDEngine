#pragma once

struct BuiltInShader
{
    const char* builtInName;
    const void* sourceCode;

    static BuiltInShader ERROR_SHADER;

private:
    BuiltInShader( const char* name, const char* source );
};
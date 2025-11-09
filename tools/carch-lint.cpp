// Carch Linter
// Style and best practice checker for Carch schemas

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/parser/ast.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

struct LintIssue {
    enum class Severity { Warning, Error };
    
    Severity severity;
    size_t line;
    size_t column;
    std::string message;
    std::string rule_id;
};

class Linter {
public:
    Linter(bool strict_mode = false) : strict_mode_(strict_mode) {}
    
    std::vector<LintIssue> lint(const std::string& source, const std::string& filename) {
        issues_.clear();
        filename_ = filename;
        
        // Lex and parse
        carch::lexer::Lexer lexer(source);
        carch::parser::Parser parser(lexer);
        auto schema = parser.parse();
        
        if (parser.has_errors()) {
            // Can't lint if there are parse errors
            return issues_;
        }
        
        // Run lint checks
        check_naming_conventions(schema.get());
        check_complexity(schema.get());
        check_best_practices(schema.get());
        
        return issues_;
    }
    
private:
    bool strict_mode_;
    std::string filename_;
    std::vector<LintIssue> issues_;
    
    void add_warning(size_t line, size_t col, const std::string& msg, const std::string& rule) {
        auto severity = strict_mode_ ? LintIssue::Severity::Error : LintIssue::Severity::Warning;
        issues_.push_back({severity, line, col, msg, rule});
    }
    
    bool is_pascal_case(const std::string& name) {
        if (name.empty() || !std::isupper(name[0])) return false;
        
        for (size_t i = 1; i < name.size(); ++i) {
            if (name[i] == '_') return false;
        }
        return true;
    }
    
    bool is_snake_case(const std::string& name) {
        if (name.empty() || std::isupper(name[0])) return false;
        
        for (char c : name) {
            if (!std::islower(c) && !std::isdigit(c) && c != '_') return false;
        }
        return true;
    }
    
    void check_naming_conventions(carch::parser::SchemaNode* schema) {
        if (!schema) return;
        
        for (const auto& type_def : schema->definitions) {
            const std::string& type_name = type_def->name;
            
            // Type names should be PascalCase
            if (!is_pascal_case(type_name)) {
                add_warning(type_def->line, type_def->column,
                    "Type name '" + type_name + "' should be PascalCase",
                    "naming-convention");
            }
            
            // Check field naming in structs
            if (auto* struct_type = dynamic_cast<carch::parser::StructTypeNode*>(type_def->type.get())) {
                for (const auto& field : struct_type->fields) {
                    if (!is_snake_case(field->name)) {
                        add_warning(type_def->line, type_def->column,
                            "Field name '" + field->name + "' should be snake_case",
                            "naming-convention");
                    }
                }
            }
        }
    }
    
    void check_complexity(carch::parser::SchemaNode* schema) {
        if (!schema) return;
        
        for (const auto& type_def : schema->definitions) {
            // Check for overly complex structs
            if (auto* struct_type = dynamic_cast<carch::parser::StructTypeNode*>(type_def->type.get())) {
                if (struct_type->fields.size() > 50) {
                    add_warning(type_def->line, type_def->column,
                        "Struct '" + type_def->name + "' has " + std::to_string(struct_type->fields.size()) +
                        " fields. Consider breaking it into smaller structs.",
                        "complexity");
                }
            }
            
            // Check for overly complex variants
            if (auto* variant_type = dynamic_cast<carch::parser::VariantTypeNode*>(type_def->type.get())) {
                if (variant_type->alternatives.size() > 20) {
                    add_warning(type_def->line, type_def->column,
                        "Variant '" + type_def->name + "' has " + std::to_string(variant_type->alternatives.size()) +
                        " alternatives. Consider restructuring.",
                        "complexity");
                }
            }
            
            // Check for overly large enums
            if (auto* enum_type = dynamic_cast<carch::parser::EnumTypeNode*>(type_def->type.get())) {
                if (enum_type->values.size() > 100) {
                    add_warning(type_def->line, type_def->column,
                        "Enum '" + type_def->name + "' has " + std::to_string(enum_type->values.size()) +
                        " values. Consider using a different representation.",
                        "complexity");
                }
            }
        }
    }
    
    void check_best_practices(carch::parser::SchemaNode* schema) {
        if (!schema) return;
        
        // Check for unused types (types that are never referenced)
        // This would require more sophisticated analysis
        
        // Check for overly nested inline types
        // This would require AST traversal depth checking
        
        // For now, just add placeholder logic
    }
};

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: carch-lint [--strict] <file.carch> [<file2.carch> ...]\n";
        std::cerr << "\nOptions:\n";
        std::cerr << "  --strict    Treat warnings as errors\n";
        return 1;
    }
    
    bool strict_mode = false;
    std::vector<std::string> files;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--strict") {
            strict_mode = true;
        } else {
            files.push_back(arg);
        }
    }
    
    if (files.empty()) {
        std::cerr << "Error: No input files specified\n";
        return 1;
    }
    
    Linter linter(strict_mode);
    int total_issues = 0;
    int total_errors = 0;
    
    for (const auto& file : files) {
        try {
            std::string source = read_file(file);
            auto issues = linter.lint(source, file);
            
            for (const auto& issue : issues) {
                const char* severity_str = (issue.severity == LintIssue::Severity::Error) ? "error" : "warning";
                
                std::cout << file << ":" << issue.line << ":" << issue.column << ": "
                         << severity_str << ": " << issue.message
                         << " [" << issue.rule_id << "]\n";
                
                total_issues++;
                if (issue.severity == LintIssue::Severity::Error) {
                    total_errors++;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing " << file << ": " << e.what() << "\n";
            return 1;
        }
    }
    
    std::cout << "\n";
    if (total_issues == 0) {
        std::cout << "✓ No issues found\n";
        return 0;
    } else {
        std::cout << total_issues << " issue(s) found";
        if (total_errors > 0) {
            std::cout << " (" << total_errors << " error(s))";
        }
        std::cout << "\n";
        return (total_errors > 0) ? 1 : 0;
    }
}

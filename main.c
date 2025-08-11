#include <stdlib.h>
#include <stdio.h>

#include "aids.h"
#include "markdown.h"

#define EXAMPLE_PATH "./example.md"

#define INDENTATION "  "

static void print_indentation(size_t indent_level) {
    for (size_t i = 0; i < indent_level; i++) {
        printf("%s", INDENTATION);
    }
}

static void print_text(const Markdown_Text *text, size_t indent_level) {
    print_indentation(indent_level); printf("Text: %.*s\n", (int)text->value.len, text->value.str);
}

static void print_phrasing_content(const Markdown_Phrasing_Content *content, size_t indent_level);

static void print_children(const Aids_Array *children, size_t indent_level) {
    for (size_t i = 0; i < children->count; i++) {
        Markdown_Phrasing_Content *content = NULL;
        if (aids_array_get(children, i, (unsigned char **)&content) != AIDS_OK) {
            aids_log(AIDS_ERROR, "Failed to get phrasing content at index %zu", i);
            exit(EXIT_FAILURE);
        }
        print_phrasing_content(content, indent_level);
    }
}

static void print_phrasing_content(const Markdown_Phrasing_Content *content, size_t indent_level) {
    switch (content->kind) {
        case MD_LINK:
            print_indentation(indent_level); printf("Link:\n");
            print_indentation(indent_level + 1); printf("Children:\n");
            print_children(&content->link.children, indent_level + 2);
            print_indentation(indent_level + 1); printf("URL: %.*s\n", (int)content->link.url.len, content->link.url.str);
            if (content->link.title.str != NULL) {
                print_indentation(indent_level + 1); printf("Title: %.*s\n", (int)content->link.title.len, content->link.title.str);
            } else {
                print_indentation(indent_level + 1); printf("Title: (none)\n");
            }
            break;
        case MD_TEXT:
            print_text(&content->text, indent_level);
            break;
        default:
            aids_log(AIDS_ERROR, "Unknown phrasing content kind");
            exit(EXIT_FAILURE);
    }
}

static void print_heading(const Markdown_Heading *heading, size_t indent_level) {
    print_indentation(indent_level); printf("Heading (depth: %lu):\n", heading->depth);
    print_indentation(indent_level + 1); printf("Children:\n");
    print_children(&heading->children, indent_level + 2);
}

static void print_paragraph(const Markdown_Paragraph *paragraph, size_t indent_level) {
    print_indentation(indent_level); printf("Paragraph:\n");
    print_indentation(indent_level + 1); printf("Children:\n");
    print_children(&paragraph->children, indent_level + 2);
}

static void print_flow_content(const Markdown_Flow_Content *flow_content, size_t indent_level) {
    switch (flow_content->kind) {
        case MD_HEADING:
            print_heading(&flow_content->heading, indent_level);
            break;
        case MD_PARAGRAPH:
            print_paragraph(&flow_content->paragraph, indent_level);
            break;
        default:
            aids_log(AIDS_ERROR, "Unknown flow content kind");
            exit(EXIT_FAILURE);
    }
}

static void print_root(const Markdown_Root *root, int indent_level) {
    for (size_t i = 0; i < root->children.count; i++) {
        Markdown_Flow_Content *flow_content = NULL;
        if (aids_array_get(&root->children, i, (unsigned char **)&flow_content) != AIDS_OK) {
            aids_log(AIDS_ERROR, "Failed to get flow content at index %zu", i);
            exit(EXIT_FAILURE);
        }

        print_indentation(indent_level); printf("Flow Content %zu:\n", i);
        print_flow_content(flow_content, indent_level + 1);
    }
}

int main() {
    Aids_String_Slice filename = aids_string_slice_from_cstr(EXAMPLE_PATH);
    Aids_String_Slice content = {0};

    if (aids_io_read(&filename, &content, "r") != AIDS_OK) {
        aids_log(AIDS_ERROR, "Failed to read file %s", aids_failure_reason());
        exit(1);
    }

    Markdown_Root root = {0};
    markdown_parse(content, &root);
    print_root(&root, 0);

    return 0;
}

#define AIDS_IMPLEMENTATION
#include "aids.h"
#define MARKDOWN_IMPLEMENTATION
#include "markdown.h"

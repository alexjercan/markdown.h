#include <stdlib.h>
#include <stdio.h>

#include "aids.h"
#include "markdown.h"

#define EXAMPLE_PATH "./README.md"

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
        if (aids_array_get(children, i, (void **)&content) != AIDS_OK) {
            aids_log(AIDS_ERROR, "Failed to get phrasing content at index %zu", i);
            exit(EXIT_FAILURE);
        }
        print_phrasing_content(content, indent_level);
    }
}

static void print_phrasing_content(const Markdown_Phrasing_Content *content, size_t indent_level) {
    switch (content->kind) {
        case MD_BREAK:
            print_indentation(indent_level); printf("Break:\n");
            print_indentation(indent_level + 1); printf("Value: %.*s\n", (int)content->br.value.len, content->br.value.str);
            break;
        case MD_EMPHASIS:
            print_indentation(indent_level); printf("Emphasis:\n");
            print_indentation(indent_level + 1); printf("Children:\n");
            print_children(&content->emphasis.children, indent_level + 2);
            break;
        case MD_HTML:
            print_indentation(indent_level); printf("HTML:\n");
            print_indentation(indent_level + 1); printf("Value: %.*s\n", (int)content->html.value.len, content->html.value.str);
            break;
        case MD_IMAGE:
            print_indentation(indent_level); printf("Image:\n");
            print_indentation(indent_level + 1); printf("Alt Text: %.*s\n", (int)content->image.alt.len, content->image.alt.str);
            print_indentation(indent_level + 1); printf("URL: %.*s\n", (int)content->image.url.len, content->image.url.str);
            if (content->image.title.str != NULL) {
                print_indentation(indent_level + 1); printf("Title: %.*s\n", (int)content->image.title.len, content->image.title.str);
            } else {
                print_indentation(indent_level + 1); printf("Title: (none)\n");
            }
            break;
        case MD_IMAGE_REFERENCE:
            print_indentation(indent_level); printf("Image Reference:\n");
            print_indentation(indent_level + 1); printf("Alt Text: %.*s\n", (int)content->image_reference.alt.len, content->image_reference.alt.str);
            if (content->image_reference.reference.str != NULL) {
                print_indentation(indent_level + 1); printf("Reference: %.*s\n", (int)content->image_reference.reference.len, content->image_reference.reference.str);
            } else {
                print_indentation(indent_level + 1); printf("Reference: (none)\n");
            }
            break;
        case MD_INLINE_CODE:
            print_indentation(indent_level); printf("Inline Code:\n");
            print_indentation(indent_level + 1); printf("Value: %.*s\n", (int)content->inline_code.value.len, content->inline_code.value.str);
            break;
        case MD_INLINE_MATH:
            print_indentation(indent_level); printf("Inline Math:\n");
            print_indentation(indent_level + 1); printf("Value: %.*s\n", (int)content->inline_math.value.len, content->inline_math.value.str);
            break;
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
        case MD_LINK_REFERENCE:
            print_indentation(indent_level); printf("Link Reference:\n");
            print_indentation(indent_level + 1); printf("(Note: MD_LINK_REFERENCE is in enum but not in union)\n");
            break;
        case MD_STRONG:
            print_indentation(indent_level); printf("Strong:\n");
            print_indentation(indent_level + 1); printf("Children:\n");
            print_children(&content->strong.children, indent_level + 2);
            break;
        case MD_TEXT:
            print_text(&content->text, indent_level);
            break;
        default:
            aids_log(AIDS_ERROR, "Unknown phrasing content kind %d", content->kind);
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

static void print_flow_content(const Markdown_Flow_Content *flow_content, size_t indent_level);
static void print_blockquote(const Markdown_Blockquote *blockquote, size_t indent_level);

static void print_flow_children(const Aids_Array *children, size_t indent_level) {
    for (size_t i = 0; i < children->count; i++) {
        Markdown_Flow_Content *content = NULL;
        if (aids_array_get(children, i, (void **)&content) != AIDS_OK) {
            aids_log(AIDS_ERROR, "Failed to get flow content at index %zu", i);
            exit(EXIT_FAILURE);
        }
        print_flow_content(content, indent_level);
    }
}

static void print_blockquote(const Markdown_Blockquote *blockquote, size_t indent_level) {
    print_indentation(indent_level); printf("Blockquote:\n");
    print_indentation(indent_level + 1); printf("Children:\n");
    print_flow_children(&blockquote->children, indent_level + 2);
}

static void print_list_item(const Markdown_List_Item *item, size_t indent_level) {
    print_indentation(indent_level); printf("List Item:\n");
    print_indentation(indent_level + 1); printf("Spread: %s\n", item->spread ? "true" : "false");
    print_indentation(indent_level + 1); printf("Children:\n");
    print_flow_children(&item->children, indent_level + 2);
}

static void print_list(const Markdown_List *list, size_t indent_level) {
    print_indentation(indent_level); printf("List:\n");
    print_indentation(indent_level + 1); printf("Ordered: %s\n", list->ordered ? "true" : "false");
    if (list->ordered) {
        print_indentation(indent_level + 1); printf("Start: %zu\n", list->start);
    }
    print_indentation(indent_level + 1); printf("Spread: %s\n", list->spread ? "true" : "false");
    print_indentation(indent_level + 1); printf("Items:\n");
    for (size_t i = 0; i < list->children.count; i++) {
        Markdown_List_Item *item = NULL;
        if (aids_array_get(&list->children, i, (void **)&item) != AIDS_OK) {
            aids_log(AIDS_ERROR, "Failed to get list item at index %zu", i);
            exit(EXIT_FAILURE);
        }
        print_list_item(item, indent_level + 2);
    }
}

static void print_definition(const Markdown_Definition *definition, size_t indent_level) {
    print_indentation(indent_level); printf("Definition:\n");
    print_indentation(indent_level + 1); printf("Identifier: %.*s\n", (int)definition->identifier.len, definition->identifier.str);
    if (definition->label.str != NULL) {
        print_indentation(indent_level + 1); printf("Label: %.*s\n", (int)definition->label.len, definition->label.str);
    } else {
        print_indentation(indent_level + 1); printf("Label: (none)\n");
    }
    print_indentation(indent_level + 1); printf("URL: %.*s\n", (int)definition->url.len, definition->url.str);
    if (definition->title.str != NULL) {
        print_indentation(indent_level + 1); printf("Title: %.*s\n", (int)definition->title.len, definition->title.str);
    } else {
        print_indentation(indent_level + 1); printf("Title: (none)\n");
    }
}

static void print_code(const Markdown_Code *code, size_t indent_level) {
    print_indentation(indent_level); printf("Code:\n");
    if (code->lang.str != NULL && code->lang.len > 0) {
        print_indentation(indent_level + 1); printf("Language: %.*s\n", (int)code->lang.len, code->lang.str);
    } else {
        print_indentation(indent_level + 1); printf("Language: (none)\n");
    }
    if (code->meta.str != NULL && code->meta.len > 0) {
        print_indentation(indent_level + 1); printf("Meta: %.*s\n", (int)code->meta.len, code->meta.str);
    } else {
        print_indentation(indent_level + 1); printf("Meta: (none)\n");
    }
    print_indentation(indent_level + 1); printf("Value: %.*s\n", (int)code->value.len, code->value.str);
}

static void print_math(const Markdown_Math *math, size_t indent_level) {
    print_indentation(indent_level); printf("Math:\n");
    print_indentation(indent_level + 1); printf("Value: %.*s\n", (int)math->value.len, math->value.str);
}

static void print_frontmatter(const Markdown_Frontmatter *frontmatter, size_t indent_level) {
    print_indentation(indent_level); printf("Frontmatter:\n");
    print_indentation(indent_level + 1); printf("Value: %.*s\n", (int)frontmatter->value.len, frontmatter->value.str);
}

static void print_flow_content(const Markdown_Flow_Content *flow_content, size_t indent_level) {
    switch (flow_content->kind) {
        case MD_BLOCKQUOTE:
            print_blockquote(&flow_content->blockquote, indent_level);
            break;
        case MD_CODE:
            print_code(&flow_content->code, indent_level);
            break;
        case MD_HEADING:
            print_heading(&flow_content->heading, indent_level);
            break;
        case MD_LIST:
            print_list(&flow_content->list, indent_level);
            break;
        case MD_THEMATIC_BREAK:
            print_indentation(indent_level); printf("Thematic Break\n");
            break;
        case MD_DEFINITION:
            print_definition(&flow_content->definition, indent_level);
            break;
        case MD_PARAGRAPH:
            print_paragraph(&flow_content->paragraph, indent_level);
            break;
        case MD_MATH:
            print_math(&flow_content->math, indent_level);
            break;
        case MD_FRONTMATTER:
            print_frontmatter(&flow_content->frontmatter, indent_level);
            break;
        default:
            aids_log(AIDS_ERROR, "Unknown flow content kind %d", flow_content->kind);
            exit(EXIT_FAILURE);
    }
}

static void print_root(const Markdown_Root *root, int indent_level) {
    for (size_t i = 0; i < root->children.count; i++) {
        Markdown_Flow_Content *flow_content = NULL;
        if (aids_array_get(&root->children, i, (void **)&flow_content) != AIDS_OK) {
            aids_log(AIDS_ERROR, "Failed to get flow content at index %zu", i);
            exit(EXIT_FAILURE);
        }

        print_indentation(indent_level); printf("Flow Content %zu:\n", i);
        print_flow_content(flow_content, indent_level + 1);
    }
}

int main(int argc, char **argv) {
    const char *filepath = EXAMPLE_PATH;

    if (argc > 1) {
        filepath = argv[1];
    }

    Aids_String_Slice filename = aids_string_slice_from_cstr(filepath);
    Aids_String_Slice content = {0};

    if (aids_io_read(&filename, &content, "r") != AIDS_OK) {
        aids_log(AIDS_ERROR, "Failed to read file %s: %s", filepath, aids_failure_reason());
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

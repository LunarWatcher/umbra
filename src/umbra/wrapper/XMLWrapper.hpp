#pragma once

#include <libxml/parser.h> // NOLINT(misc-header-include-cycle): can't fix
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <stdexcept>

// TODO: actually do proper error handling
namespace umbra {

namespace {

inline void ensureLoaded() {
    static bool loaded = false;
    if (loaded) {
        return;
    }
    xmlInitParser();
    loaded = true;
}

}

struct XMLParserContext {
    xmlParserCtxtPtr context;

    XMLParserContext() {
        context = xmlNewParserCtxt();
        if (context == nullptr) {
            throw std::runtime_error("Failed to allocate libxml parser context");
        }
    }
    ~XMLParserContext() {
        xmlFreeParserCtxt(context);
    }
};

struct XPathContext {
    xmlXPathContextPtr xpathContext;
    xmlXPathObjectPtr result;
    XPathContext(
        xmlDocPtr doc,
        xmlNode* start,
        const xmlChar* expr
    ) {
        xpathContext = xmlXPathNewContext(doc);
        xpathContext->node = start;
        result = xmlXPathEvalExpression(
            expr, xpathContext
        );
    }
    ~XPathContext() {
        if (xpathContext != nullptr) {
            xmlXPathFreeContext(xpathContext);
        }
        if (result != nullptr) {
            xmlXPathFreeObject(result);
        }
    }

    XPathContext(XPathContext&& other) noexcept
        : xpathContext(other.xpathContext),
          result(other.result)
    {
        other.xpathContext = nullptr;
        other.result = nullptr;
    }
    XPathContext(const XPathContext&) = delete;

    xmlNode* autoresolve() {
        if (
            result == nullptr
            || xmlXPathNodeSetIsEmpty(result->nodesetval)
        ) {
            throw std::runtime_error("Failed to resolve xpath");
        }
        return result->nodesetval->nodeTab[0];
    }
    xmlNode* autoresolveNullable() {
        if (
            result == nullptr
            || xmlXPathNodeSetIsEmpty(result->nodesetval)
        ) {
            return nullptr;
        }
        return result->nodesetval->nodeTab[0];
    }
};

struct XMLDocumentContext {
    XMLParserContext ctx;
    xmlDocPtr doc;
    xmlNodePtr root;

    XMLDocumentContext(const std::string& filename) {
        doc = xmlCtxtReadFile(
            ctx.context,
            filename.c_str(),
            nullptr,
            0
        );
        if (doc == nullptr) {
            throw std::runtime_error(
                "Failed to parse " + filename
            );
        }
        root = xmlDocGetRootElement(doc);
    }
    ~XMLDocumentContext() {
        xmlFreeDoc(doc);
    }

    XPathContext xpath(
        xmlNode* start,
        const xmlChar* expr
    ) {
        return XPathContext {
            doc, start, expr
        };
    }
};

}

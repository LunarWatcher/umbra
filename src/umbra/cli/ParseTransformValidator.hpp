#pragma once

#include "CLI/CLI.hpp"
#include "umbra/util/Parse.hpp"

namespace umbra {

struct ParseTransformValidator : public CLI::Validator {
    ParseTransformValidator(
        const parse::ParseContext& ctx
    ) : Validator(
        // TODO: Copy isn't great, but I don't know if we can take `&ctx` without running into weird shit
        // I would assume `ParseContext` to be stable, or at the very least the outer class to be, but I don't know if
        // there's other shit that could cause UB here, so let's just copy for now.
        [ctx](std::string& val) -> std::string {
            val = parse::parse(val, ctx);
            std::cout << "hi" << std::endl;
            return {};
        },
        // The description is the outward-facing name in positionals for whatever reason, so this can't actually be a
        // description. This results in
        // ```
        // layout TEXT:Supports templates [default.kdl] 
        //                             Path or name of the layout 
        // ```
        // Which is _perfect_
        "Supports templates",
        "ParseTransform"
    ) {}
};

}

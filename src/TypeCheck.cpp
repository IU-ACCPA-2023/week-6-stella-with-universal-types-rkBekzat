#include <iostream>
#include "TypeCheck.h"
#include "Visiting.h"

namespace Stella {

void typecheckProgram(Program *program) {
    program->accept(new Visiting());
    return;
}

} // namespace Stella

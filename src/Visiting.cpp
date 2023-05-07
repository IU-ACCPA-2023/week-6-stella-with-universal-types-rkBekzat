//
// Created by bekzat on 27.03.23.
//

#include "Visiting.h"
#include <iostream>

namespace Stella
{
    void Visiting::visitProgram(Program *t) {}                   // abstract class
    void Visiting::visitLanguageDecl(LanguageDecl *t) {}         // abstract class
    void Visiting::visitExtension(Extension *t) {}               // abstract class
    void Visiting::visitDecl(Decl *t) {}                         // abstract class
    void Visiting::visitLocalDecl(LocalDecl *t) {}               // abstract class
    void Visiting::visitAnnotation(Annotation *t) {}             // abstract class
    void Visiting::visitParamDecl(ParamDecl *t) {}               // abstract class
    void Visiting::visitReturnType(ReturnType *t) {}             // abstract class
    void Visiting::visitThrowType(ThrowType *t) {}               // abstract class
    void Visiting::visitExpr(Expr *t) {}                         // abstract class
    void Visiting::visitMatchCase(MatchCase *t) {}               // abstract class
    void Visiting::visitOptionalTyping(OptionalTyping *t) {}     // abstract class
    void Visiting::visitPatternData(PatternData *t) {}           // abstract class
    void Visiting::visitExprData(ExprData *t) {}                 // abstract class
    void Visiting::visitPattern(Pattern *t) {}                   // abstract class
    void Visiting::visitLabelledPattern(LabelledPattern *t) {}   // abstract class
    void Visiting::visitBinding(Binding *t) {}                   // abstract class
    void Visiting::visitType(Type *t) {}                         // abstract class
    void Visiting::visitPatternBinding(PatternBinding *t) {}     // abstract class 
    void Visiting::visitVariantFieldType(VariantFieldType *t) {} // abstract class
    void Visiting::visitRecordFieldType(RecordFieldType *t) {}   // abstract class
    void Visiting::visitTyping(Typing *t) {}                     // abstract class

    void Visiting::visitAProgram(AProgram *a_program)
    {
        /* Code For AProgram Goes Here */
        std::cout << "Start Program\n";
        increaseScope();
        if (a_program->languagedecl_)
            a_program->languagedecl_->accept(this);
        if (a_program->listextension_)
            a_program->listextension_->accept(this);
        if (a_program->listdecl_)
            a_program->listdecl_->accept(this);
        decreaseScope();
    }

    void Visiting::visitLanguageCore(LanguageCore *language_core)
    {
        /* Code For LanguageCore Goes Here */
        std::cout << "visitLanguageCore\n";
    }

    void Visiting::visitAnExtension(AnExtension *an_extension)
    {
        /* Code For AnExtension Goes Here */
        std::cout << "visitAnExtension";
        if (an_extension->listextensionname_)
            an_extension->listextensionname_->accept(this);
    }


    void Visiting::visitDeclFun(DeclFun *decl_fun)
    {
        /* Code For DeclFun Goes Here */
        std::cout << "Visiting function declaration for " << decl_fun->stellaident_ << "\n";
        increaseScope();
        ObjectType objFunc(MyTypeTag::FunctionTypeTag); // first I create objectType to store all types and in the end add to context
        if (decl_fun->listannotation_)
            decl_fun->listannotation_->accept(this);

        int sizedBefore = contexts.size();
        if (decl_fun->listparamdecl_)
            decl_fun->listparamdecl_->accept(this);

        while(sizedBefore < contexts.size()){
            objFunc.params.push_back(contexts.top());
            contexts.pop();
        }

        if (decl_fun->returntype_)
            decl_fun->returntype_->accept(this);

        objFunc.returns.push_back(contexts.top());
        contexts.pop();

        if (decl_fun->throwtype_)
            decl_fun->throwtype_->accept(this);
        if (decl_fun->listdecl_)
            decl_fun->listdecl_->accept(this);
        if (decl_fun->expr_)
            decl_fun->expr_->accept(this);

        std::cout << "Fucntion go out: "<< objFunc.typeTag << " "<<  objFunc.returns[0].typeTag << " " << contexts.top().typeTag << " " << contexts.size() << "\n" ;
        std::cout << "Function name: " << decl_fun->stellaident_ << "\n";
        // checking expected return type and actual return type same or not
        // add new condition if actual return type is panic then skip
        if(!checkReturn(contexts.top(), objFunc.returns[0]) && contexts.top().typeTag != MyTypeTag::PanicTypeTag){
            std::cout << "ERROR: function should return another type\n";
            std::cout << "Line: " << decl_fun->expr_->line_number << "\n";
            exit(1);
        }
        // map the type for identifier
        decreaseScope();
        contextIdent[decl_fun->stellaident_].push(objFunc);
        scopedContext.top().push_back(decl_fun->stellaident_);
        visitStellaIdent(decl_fun->stellaident_);

    }

    void Visiting::visitDeclTypeAlias(DeclTypeAlias *decl_type_alias)
    {
        /* Code For DeclTypeAlias Goes Here */

        std::cout << "DECL TYPE ALIAS: " << decl_type_alias->char_number << " " << decl_type_alias->line_number << "\n";
        visitStellaIdent(decl_type_alias->stellaident_);


        if (decl_type_alias->type_)
            decl_type_alias->type_->accept(this);
    }

    void Visiting::visitDeclExceptionType(DeclExceptionType *decl_exception_type)
    {
        /* Code For DeclExceptionType Goes Here */
        std::cout << "visitDeclExceptionType\n";
        if (decl_exception_type->type_)
            decl_exception_type->type_->accept(this);
    }

    void Visiting::visitDeclExceptionVariant(DeclExceptionVariant *decl_exception_variant)
    {
        /* Code For DeclExceptionVariant Goes Here */
        std::cout << "visitDeclExceptionVariant\n";
        visitStellaIdent(decl_exception_variant->stellaident_);
        if (decl_exception_variant->type_)
            decl_exception_variant->type_->accept(this);
    }

    void Visiting::visitAssign(Assign *assign)
    {
        /* Code For Assign Goes Here */
        std::cout << "visitAssign\n";
        if (assign->expr_1)
            assign->expr_1->accept(this);
        ObjectType First = contexts.top();
        contexts.pop();
        if (assign->expr_2)
            assign->expr_2->accept(this);
        if(contexts.top().typeTag != First.params[0].typeTag ){
            std::cout << "ERROR: you can't change type, on line: " << assign->line_number << "\n";
            exit(1);
        }
        contexts.pop();
        contexts.push(ObjectType(MyTypeTag::UnitTypeTag));
    }

    void Visiting::visitRef(Ref *ref)
    {
        /* Code For Ref Goes Here */
        std::cout << "visitRef\n";
        if (ref->expr_)
            ref->expr_->accept(this);
        ObjectType objRef = ObjectType(MyTypeTag::ReferenceTypeTag);
        objRef.params.push_back(contexts.top());
        contexts.pop();
        contexts.push(objRef);
    }

    void Visiting::visitDeref(Deref *deref)
    {
        /* Code For Deref Goes Here */
        std::cout << "visitDeref\n";
        if (deref->expr_)
            deref->expr_->accept(this);
        if(contexts.top().typeTag != MyTypeTag::ReferenceTypeTag){
            std::cout << "ERROR: The type should be reference, on line: " << deref->line_number << "\n";
            exit(1);
        }
        ObjectType objDeref = contexts.top().params[0];
        contexts.pop();
        contexts.push(objDeref);
    }

    void Visiting::visitPanic(Panic *panic)
    {
        /* Code For Panic Goes Here */
        std::cout << "visitPanic\n";
        contexts.push(ObjectType(MyTypeTag::PanicTypeTag));
    }

    void Visiting::visitThrow(Throw *throw_)
    {
        /* Code For Throw Goes Here */
        std::cout << "visitThrow\n";
        if (throw_->expr_)
            throw_->expr_->accept(this);
    }

    void Visiting::visitTryCatch(TryCatch *try_catch)
    {
        /* Code For TryCatch Goes Here */
        std::cout << "visitTryCatch";
        if (try_catch->expr_1)
            try_catch->expr_1->accept(this);
        if (try_catch->pattern_)
            try_catch->pattern_->accept(this);
        if (try_catch->expr_2)
            try_catch->expr_2->accept(this);
    }

    void Visiting::visitTryWith(TryWith *try_with)
    {
        /* Code For TryWith Goes Here */
        std::cout << "visitTryWith\n";
        if (try_with->expr_1)
            try_with->expr_1->accept(this);
        if (try_with->expr_2)
            try_with->expr_2->accept(this);
    }

    void Visiting::visitALocalDecl(ALocalDecl *a_local_decl)
    {
        /* Code For ALocalDecl Goes Here */
        std::cout << "AlocalDecl\n";
        if (a_local_decl->decl_)
            a_local_decl->decl_->accept(this);
    }

    void Visiting::visitInlineAnnotation(InlineAnnotation *inline_annotation)
    {
        /* Code For InlineAnnotation Goes Here */
        std::cout << "visitInlineAnnotation\n";

    }

    void Visiting::visitAParamDecl(AParamDecl *a_param_decl)
    {
        /* Code For AParamDecl Goes Here */
        std::cout << "visitAParamDecl\n";

        if (a_param_decl->type_)
            a_param_decl->type_->accept(this);

        std::cout <<"SET the stellIDENT: "<< a_param_decl->stellaident_ << " " << contexts.top().typeTag << "\n\n";
        contextIdent[a_param_decl->stellaident_].push(contexts.top());
        scopedContext.top().push_back(a_param_decl->stellaident_); // add the list which store stellaidents
        visitStellaIdent(a_param_decl->stellaident_);
        contexts.pop();
    }

    void Visiting::visitNoReturnType(NoReturnType *no_return_type)
    {
        /* Code For NoReturnType Goes Here */
        std::cout << "visitNoReturnType\n";
    }

    void Visiting::visitSomeReturnType(SomeReturnType *some_return_type)
    {
        /* Code For SomeReturnType Goes Here */
        std::cout << "SomereturnType\n";
        if (some_return_type->type_)
            some_return_type->type_->accept(this);
    }

    void Visiting::visitNoThrowType(NoThrowType *no_throw_type)
    {
        /* Code For NoThrowType Goes Here */
        std::cout << "visitNoThrowType\n";
    }

    void Visiting::visitSomeThrowType(SomeThrowType *some_throw_type)
    {
        /* Code For SomeThrowType Goes Here */
        std::cout << "visitSomeThrowType\n";
        if (some_throw_type->listtype_)
            some_throw_type->listtype_->accept(this);
    }

    void Visiting::visitIf(If *if_)
    {
        /* Code For If Goes Here */
        std::cout << "VisitIf\n";
        if (if_->expr_1)
            if_->expr_1->accept(this);
        // here in contexts.top() will store type of expr1
        // how I check undefined, look at function visitStellaIdent() here I explained
        if(contexts.top().typeTag == MyTypeTag::UndefinedTag){
            std::cout << "ERROR: The variable undefined, on line: " << if_->expr_1->line_number << " at position: " << if_->expr_1->char_number << "\n";
            exit(1);
        }
        // check proper type(bool)
        // also check if panic then skip
        if(MyTypeTag::PanicTypeTag != contexts.top().typeTag && MyTypeTag::BoolTypeTag != contexts.top().typeTag){
            std::cout << "ERROR: if condition type should be bool. On line:" << if_->expr_1->line_number << " char:" << if_->expr_1->char_number << "\n";
            exit(1);
        }
        // delete this type, no need to store somewhere
        contexts.pop();

        increaseScope();
        if (if_->expr_2)
            if_->expr_2->accept(this);
        // here in contexts.top() will store type of expr2
        expected_type = contexts.top(); // I store this type cause it's need to check with third expr type
        contexts.pop(); // delete this type

        if(expected_type.typeTag == MyTypeTag::UndefinedTag){ // check if this undefined type
            std::cout << "ERROR: Undefined type on line: " << if_->expr_2->line_number << " at position: " << if_->expr_2->char_number << "\n";
            exit(1);
        }
        decreaseScope();
        increaseScope();
        if (if_->expr_3)
            if_->expr_3->accept(this);
        // here in context.top() will store type of expr3
//        std::cout << "BEFORE CHECKING IF: " << contexts.top().typeTag << " " << expected_type.typeTag << "\n";
        if(contexts.top().typeTag == MyTypeTag::UndefinedTag){ // checking
            std::cout << "ERROR: Undefined type on line: " << if_->expr_3->line_number << " at position: " << if_->expr_3->char_number << "\n";
            exit(1);
        }
        if(expected_type.typeTag == MyTypeTag::PanicTypeTag){
            // if first expr type is panic
            // it's mean we take second expr type as return
            return ;
        }
        if(contexts.top().typeTag == MyTypeTag::PanicTypeTag){
            // if second expr type is panic
            // then remove second from stack context
            // add expected type
            contexts.pop();
            contexts.push(expected_type);
            return ;
        }

        // checking second expr type and third expr type
        if(contexts.empty() || expected_type != contexts.top()){
            std:: cout << expected_type.typeTag << " " << contexts.top().typeTag << "\n";
            std::cout << "ERROR: if return different types on line: " << if_->expr_2->line_number << " position "<<  if_->expr_2->char_number ;
            std::cout<< " and on line:" << if_->expr_3->line_number << " postion" << if_->expr_3->char_number  << "\n";
            exit(1);
        }
        decreaseScope();
        // I not remove element from stack cause I need result
    }

    void Visiting::visitLet(Let *let)
    {
        /* Code For Let Goes Here */
        std::cout << "visitLet\n";
        if (let->listpatternbinding_)
            let->listpatternbinding_->accept(this);
        if (let->expr_)
            let->expr_->accept(this);
    }

    void Visiting::visitLetRec(LetRec *let_rec)
    {
        /* Code For LetRec Goes Here */
        std::cout << "visitLetRec\n";
        if (let_rec->listpatternbinding_)
            let_rec->listpatternbinding_->accept(this);
        if (let_rec->expr_)
            let_rec->expr_->accept(this);
    }

    void Visiting::visitLessThan(LessThan *less_than)
    {
        /* Code For LessThan Goes Here */
        std::cout << "LessThan\n";
        int sizeBefore = contexts.size();
        if (less_than->expr_1)
            less_than->expr_1->accept(this);
        if(contexts.size() - sizeBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: Should be Nat type, on line: " << less_than->line_number << "\n";
            exit(1);
        }
        contexts.pop();
        if (less_than->expr_2)
            less_than->expr_2->accept(this);
        if(contexts.size() - sizeBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: Should be Nat type, on line: " << less_than->line_number << "\n";
            exit(1);
        }
    }

    void Visiting::visitLessThanOrEqual(LessThanOrEqual *less_than_or_equal)
    {
        /* Code For LessThanOrEqual Goes Here */
        std::cout << "LessOrEqual\n";
        int sizedBefore = contexts.size();
        if (less_than_or_equal->expr_1)
            less_than_or_equal->expr_1->accept(this);
        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: Should be Nat type, on line: " << less_than_or_equal->line_number << "\n";
            exit(1);
        }
        contexts.pop();
        if (less_than_or_equal->expr_2)
            less_than_or_equal->expr_2->accept(this);
        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: Should be Nat type, on line: " << less_than_or_equal->line_number << "\n";
            exit(1);
        }
    }

    void Visiting::visitGreaterThan(GreaterThan *greater_than)
    {
        /* Code For GreaterThan Goes Here */
        std::cout << "visitGreaterThan\n";
        int sizedBefore = contexts.size();
        if (greater_than->expr_1)
            greater_than->expr_1->accept(this);
        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: Should be Nat type, on line: " << greater_than->line_number << "\n";
            exit(1);
        }
        contexts.pop();

        if (greater_than->expr_2)
            greater_than->expr_2->accept(this);
        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: Should be Nat type, on line: " << greater_than->line_number << "\n";
            exit(1);
        }
    }

    void Visiting::visitGreaterThanOrEqual(GreaterThanOrEqual *greater_than_or_equal)
    {
        /* Code For GreaterThanOrEqual Goes Here */
        std::cout << "GreaterOrEqual\n";
        int sizedBefore = contexts.size();
        if (greater_than_or_equal->expr_1)
            greater_than_or_equal->expr_1->accept(this);
        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: Should be Nat type, on line: " << greater_than_or_equal->line_number << "\n";
            exit(1);
        }
        contexts.pop();

        if (greater_than_or_equal->expr_2)
            greater_than_or_equal->expr_2->accept(this);
        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: Should be Nat type, on line: " << greater_than_or_equal->line_number << "\n";
            exit(1);
        }

    }

    void Visiting::visitEqual(Equal *equal)
    {
        /* Code For Equal Goes Here */
        std::cout << "visitEqual\n";
        int sizedBefore = contexts.size();
        if (equal->expr_1)
            equal->expr_1->accept(this);
        if(contexts.size() - sizedBefore != 1){
            std::cout << "ERROR: Should be one variable, on line: " << equal->line_number << "\n";
            exit(1);
        }
        ObjectType first = contexts.top();
        contexts.pop();

        if (equal->expr_2)
            equal->expr_2->accept(this);

        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != first.typeTag){
            std::cout << "ERROR: Should be one variable , on line: " << equal->line_number << "\n";
            exit(1);
        }
        contexts.pop();
        contexts.push(ObjectType(MyTypeTag::BoolTypeTag));
    }

    void Visiting::visitNotEqual(NotEqual *not_equal)
    {
        /* Code For NotEqual Goes Here */
        std::cout << "NotEqual ";
        int sizedBefore = contexts.size();
        if (not_equal->expr_1)
            not_equal->expr_1->accept(this);
        if(contexts.size() - sizedBefore != 1){
            std::cout << "ERROR: Should be Nat type, on line: " << not_equal->line_number << "\n";
            exit(1);
        }
        ObjectType first = contexts.top();
        contexts.pop();

        if (not_equal->expr_2)
            not_equal->expr_2->accept(this);
        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != first.typeTag){
            std::cout << "ERROR: Should be Nat type, on line: " << not_equal->line_number << "\n";
            exit(1);
        }
    }

    void Visiting::visitTypeAsc(TypeAsc *type_asc)
    {
        /* Code For TypeAsc Goes Here */
        std::cout << "visitTypeAsc\n";
        if (type_asc->expr_)
            type_asc->expr_->accept(this);
        if (type_asc->type_)
            type_asc->type_->accept(this);
    }

    void Visiting::visitTypeCast(TypeCast *type_cast)
    {
        /* Code For TypeCast Goes Here */

        if (type_cast->expr_)
            type_cast->expr_->accept(this);
        if (type_cast->type_)
            type_cast->type_->accept(this);
    }

    void Visiting::visitAbstraction(Abstraction *abstraction)
    {
        /* Code For Abstraction Goes Here */
        std::cout << "visitAbstraction\n";

        int sizeBefore = contexts.size(); // this variable need to recognize how many params declared
        ObjectType obj(MyTypeTag::FunctionTypeTag);
        if (abstraction->listparamdecl_)
            abstraction->listparamdecl_->accept(this);
        // here on position sizedBox and until end in stack will be params
        // so I add all the objecttype to vector
        while(contexts.size() > sizeBefore){
            obj.params.push_back(contexts.top());
            contexts.pop();
        }

        if (abstraction->expr_)
            abstraction->expr_->accept(this);
        // here contexts.top() will be return type, and as you mentioned here only one object, cause any function cat return on type
        obj.returns.push_back(contexts.top());
        contexts.pop();

        // then we add this object type to context, cause it will be needed in future
        contexts.push(obj);


    }

    void Visiting::visitTuple(Tuple *tuple)
    {
        /* Code For Tuple Goes Here */
        std::cout << "visitTuple\n";
        int sizedBefore = contexts.size();
        if (tuple->listexpr_)
            tuple->listexpr_->accept(this);
        ObjectType objTuple(MyTypeTag::TupleTypeTag);
        while(sizedBefore < contexts.size()){
            objTuple.params.push_back(contexts.top());
            contexts.pop();
        }

        contexts.push(objTuple);
    }

    void Visiting::visitRecord(Record *record)
    {
        /* Code For Record Goes Here */
        std::cout << "visitRecord\n";
        contexts.push(ObjectType(MyTypeTag::RecordsTypeTag));
        if (record->listbinding_)
            record->listbinding_->accept(this);
    }

    void Visiting::visitVariant(Variant *variant)
    {
        /* Code For Variant Goes Here */
        std::cout << "visitVariant\n";
        visitStellaIdent(variant->stellaident_);
        if (variant->exprdata_)
            variant->exprdata_->accept(this);
    }

    void Visiting::visitMatch(Match *match)
    {
        /* Code For Match Goes Here */
        std::cout << "visitMatch\n";
        if (match->expr_)
            match->expr_->accept(this);
        if(contexts.top().typeTag != MyTypeTag::SumTypeTag){
            std::cout << "ERROR: It's not sum type. On line" << match->line_number << " at position: " << match->char_number << "\n" ;
            exit(1);
        }
        // here in contexts.top() will store sum types, where params vector will store object types which we will sums
        // inl is contexts.top().params[0]
        // inr is contexts.top().params[1]

        if (match->listmatchcase_)
            match->listmatchcase_->accept(this);
    }

    void Visiting::visitList(List *list)
    {
        /* Code For List Goes Here */
        std::cout << "visitList\n";
        if (list->listexpr_)
            list->listexpr_->accept(this);
    }

    void Visiting::visitAdd(Add *add)
    {
        /* Code For Add Goes Here */
        std::cout << "visitAdd\n";
        int sizedBefore = contexts.size();
        if (add->expr_1)
            add->expr_1->accept(this);

        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: add operator work for NAT, on line: " << add->expr_1->line_number << "\n";
            exit(1);
        }
        contexts.pop();
        if (add->expr_2)
            add->expr_2->accept(this);

        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: add operator work for NAT, on line: " << add->expr_1->line_number << "\n";
            exit(1);
        }
    }

    void Visiting::visitSubtract(Subtract *subtract)
    {
        /* Code For Subtract Goes Here */
        std::cout << "visitSubtract\n";
        if (subtract->expr_1)
            subtract->expr_1->accept(this);
        if (subtract->expr_2)
            subtract->expr_2->accept(this);
    }

    void Visiting::visitLogicOr(LogicOr *logic_or)
    {
        /* Code For LogicOr Goes Here */
        std::cout << "visitLogicOr\n";
        if (logic_or->expr_1)
            logic_or->expr_1->accept(this);
        if (logic_or->expr_2)
            logic_or->expr_2->accept(this);
    }

    void Visiting::visitMultiply(Multiply *multiply)
    {
        /* Code For Multiply Goes Here */
        std::cout << "visitMultiply\n";
        if (multiply->expr_1)
            multiply->expr_1->accept(this);
        if (multiply->expr_2)
            multiply->expr_2->accept(this);
    }

    void Visiting::visitDivide(Divide *divide)
    {
        /* Code For Divide Goes Here */
        std::cout << "visitDivide\n";
        if (divide->expr_1)
            divide->expr_1->accept(this);
        if (divide->expr_2)
            divide->expr_2->accept(this);
    }

    void Visiting::visitLogicAnd(LogicAnd *logic_and)
    {
        /* Code For LogicAnd Goes Here */
        std::cout << "visitLogicAnd\n";
        if (logic_and->expr_1)
            logic_and->expr_1->accept(this);
        if (logic_and->expr_2)
            logic_and->expr_2->accept(this);
    }

    void Visiting::visitApplication(Application *application)
    {
        /* Code For Application Goes Here */
        std::cout << "visitApplication\n";
        if (application->expr_)
            application->expr_->accept(this);
        // contexts.top() should function
        if(contexts.empty() ||  contexts.top().typeTag != MyTypeTag::FunctionTypeTag){
            std::cout << "ERROR: This is not function. On line: " << application->expr_->line_number << " at position: " << application->expr_->char_number << "\n";
            exit(1);
        }
        ObjectType func = contexts.top();
        contexts.pop();
        int sizedBefore = contexts.size(); // to help determine which type I should pass to function
//        std::cout << "BEFORE LIST EXP IN APPLICATION: " << sizedBefore << " "  << obj.typeTag << "\n";
        if (application->listexpr_)
            application->listexpr_->accept(this);

        int positionOfParam = 0; // it will track for which param I pass the object
//        std::cout << "BEFORE CHECK APPLICATION: " << sizedBefore <<  " " << contexts.size() << " " << obj.returns.size() << " " <<  obj.params.size() << "\n";
        // while I go pass the list exprision I will check with type in params of function
        ObjectType args = getArgs(sizedBefore);
        if(!subTypeFunc(func, args)){
            std::cout << "ERROR: mismatch argument and not subtyping, on line: " << application->line_number << "\n";
            exit(1);
        }
//        while (contexts.size() > sizedBefore && positionOfParam < obj.params.size()){
//            if(contexts.top() != obj.params[positionOfParam]){
//                std::cout << "ERROR: mismatch this params on line: " << application->line_number << "\n";
//                std::cout << contexts.top().typeTag << " " << obj.params[positionOfParam].typeTag << "\n";
//                std::cout << contexts.top().params.size() << " " << obj.params[positionOfParam].params.size() << "\n";
//                std::cout << contexts.top().returns.size() << " " << obj.params[positionOfParam].returns.size() << "\n";
//                exit(1);
//            }
//            contexts.pop();
//            positionOfParam++;
//        }
//        // if we not reach to the end, it's mean not enough argument passed
//        if(positionOfParam < obj.params.size()){
//            std::cout << "ERROR: not enough params on line: " << application->line_number << "\n";
//            exit(1);
//        }
        std::cout << "Before end application: " << func.typeTag << " " << func.returns[0].typeTag << "\n";
        // otherwise we add to context return type of function
        contexts.push(func.returns[0]); // every time return should be 1 element hence I get 1 element
//        std::cout << "AFTER APPLICATION\n";
    }

    void Visiting::visitConsList(ConsList *cons_list)
    {
        /* Code For ConsList Goes Here */
        std::cout << "visitConsList\n";
        if (cons_list->expr_1)
            cons_list->expr_1->accept(this);
        if (cons_list->expr_2)
            cons_list->expr_2->accept(this);
    }

    void Visiting::visitHead(Head *head)
    {
        /* Code For Head Goes Here */
        std::cout << "visitHead\n";
        if (head->expr_)
            head->expr_->accept(this);
    }

    void Visiting::visitIsEmpty(IsEmpty *is_empty)
    {
        /* Code For IsEmpty Goes Here */
        std::cout << "visitIsEmpty\n";
        int sizeBefore = contexts.size();
        if (is_empty->expr_)
            is_empty->expr_->accept(this);
        if(contexts.size() - sizeBefore != 1){
            std::cout << "ERROR: IsEmpty not work proper: function should take only one expr, but take: " << contexts.size() - sizeBefore << "\n";
            exit(1);
        }
        if(contexts.top().typeTag != MyTypeTag::ListTypeTag){
            std::cout << "ERROR: IsEmpty function accept list." << contexts.top().typeTag << "\n";
            exit(1);
        }
        contexts.pop(); // deleting list
        contexts.push(ObjectType(MyTypeTag::BoolTypeTag)); // This function return bool
    }

    void Visiting::visitTail(Tail *tail)
    {
        /* Code For Tail Goes Here */
        std::cout << "visitTail";
        if (tail->expr_)
            tail->expr_->accept(this);
    }

    void Visiting::visitInl(Inl *inl)
    {
        /* Code For Inl Goes Here */

        std::cout << "visitInl\n";
        ObjectType objectSum(MyTypeTag::SumTypeTag);
        if (inl->expr_)
        inl->expr_->accept(this);
        objectSum.params.push_back(contexts.top());
        objectSum.params.push_back(ObjectType(MyTypeTag::UnitTypeTag));
        contexts.pop();
        contexts.push(objectSum);
    }

    void Visiting::visitInr(Inr *inr)
    {
        /* Code For Inr Goes Here */
        std::cout << "visitInr\n";
        ObjectType objectSum(MyTypeTag::SumTypeTag);
        objectSum.params.push_back(ObjectType(MyTypeTag::UnitTypeTag));
        if (inr->expr_)
        inr->expr_->accept(this);
        objectSum.params.push_back(contexts.top());
        contexts.pop();
        contexts.push(objectSum);
    }

    void Visiting::visitSucc(Succ *succ)
    {
        /* Code For Succ Goes Here */
        std::cout << "visitSucc\n";
        if (succ->expr_)
            succ->expr_->accept(this);

        if(contexts.empty() || contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: Succ should accept only Nat. Here different type!\n";
            std::cout << "On line " << succ->expr_->line_number << " at position: " << succ->expr_->char_number << "\n";
            exit(1);
         }
    }

    void Visiting::visitLogicNot(LogicNot *logic_not)
    {
        /* Code For LogicNot Goes Here */
        std::cout << "visitLogicNot\n";
        if (logic_not->expr_)
            logic_not->expr_->accept(this);
    }

    void Visiting::visitPred(Pred *pred)
    {
        /* Code For Pred Goes Here */
        std::cout << "visitPred\n";
        if (pred->expr_)
            pred->expr_->accept(this);
    }

    void Visiting::visitIsZero(IsZero *is_zero)
    {
        /* Code For IsZero Goes Here */
        std::cout << "visitIsZero\n";

        if (is_zero->expr_)
            is_zero->expr_->accept(this);
    }

    void Visiting::visitFix(Fix *fix)
    {
        /* Code For Fix Goes Here */
        std::cout << "visitFix\n";

        if (fix->expr_)
            fix->expr_->accept(this);
    }

    void Visiting::visitNatRec(NatRec *nat_rec)
    {
        /* Code For NatRec Goes Here */
        std::cout << "visitNatRec\n";
        ObjectType objRec(MyTypeTag::UndefinedTag); // at the beginning no info which type will return, I will set later
        int sizedBefore = contexts.size(); // help to recognize list of expresion in first expr
        if (nat_rec->expr_1)
            nat_rec->expr_1->accept(this);
        // contexts contain expr1 types
        if(contexts.size() - sizedBefore != 1 || contexts.top().typeTag != MyTypeTag::NatTypeTag){ // check It shoould one element and type nat
            std::cout << "ERROR: NatRec wrong on 1st argument on line" << nat_rec->expr_1->line_number << " at position: " << nat_rec->expr_1->char_number << "\n";
            exit(1);
        }
        // add to obj and remove from context
        objRec.params.push_back(contexts.top());
        contexts.pop();
        increaseScope();
        sizedBefore = contexts.size();
        if (nat_rec->expr_2)
            nat_rec->expr_2->accept(this);

        if(contexts.size() - sizedBefore != 1){ // again should be one argument
            std::cout << "ERROR: NatRec wrong on 2nd argument on line" << nat_rec->expr_2->line_number << " at position: " << nat_rec->expr_1->char_number << "\n";
            exit(1);
        }
        // return type should type of second expr
        objRec.params.push_back(contexts.top());
        objRec.returns.push_back(contexts.top());
        contexts.pop();
        decreaseScope();
        increaseScope();
        sizedBefore = contexts.size();

        if (nat_rec->expr_3)
            nat_rec->expr_3->accept(this);
        // context.top() is type of 3rd expr

        if(contexts.size() - sizedBefore != 1){ // third expr also should one arg
            std::cout << "ERROR: NatRec wrong on 3rd argument on line" << nat_rec->expr_3->line_number << " at position: " << nat_rec->expr_1->char_number << "\n";
            exit(1);
        }
        // by documentation
//        n has to be of type Nat;
//        z can be of any type T;
//        s has to be of type fn(Nat) -> (fn(T) -> T); at the bottom I check this rule 's'
//        again contexts.top() is s in our case

        if(contexts.top().typeTag != MyTypeTag::FunctionTypeTag){ // also this arg should be func
            std::cout << "ERROR: NatRec 3rd argument isn't function\n";
            std::cout << "Line: " << nat_rec->expr_3->line_number << " position:" << nat_rec->expr_3->char_number << "\n";
            exit(1);
        }
        // functio should accept one arg
        if(contexts.top().params.size() != 1){ //
            std::cout << "ERROR: NatRec 3rd argument function should accept one param but accept: " << contexts.top().params.size() << "\n";
            std::cout << "Line: " << nat_rec->expr_3->line_number << " position:" << nat_rec->expr_3->char_number << "\n";
            exit(1);
        }
        // return function type should be return another function
        if(contexts.top().returns[0].typeTag != MyTypeTag::FunctionTypeTag){
            std::cout << "ERROR: NatRec 3rd argument function should return another functin\n";
            std::cout << "Line: " << nat_rec->expr_3->line_number << " position:" << nat_rec->expr_3->char_number << "\n";
            exit(1);
        }
        // and returned function should same params and return types
        if(contexts.top().returns[0].params[0].typeTag != contexts.top().returns[0].returns[0].typeTag){
            std::cout << "ERROR: NatRec 3rd argument functions: Should be same type of params type and return type";
            std::cout << "Line: " << nat_rec->expr_3->line_number << " position:" << nat_rec->expr_3->char_number << "\n";
            exit(1);
        }
        // set the type  contexts.top().returns[0].params[0].typeTag is 'z' type which mention above
        objRec.typeTag = contexts.top().returns[0].params[0].typeTag;
        objRec.params.push_back(contexts.top());
        contexts.pop();
        contexts.push(objRec.returns[0]); // add obj to contexts
        decreaseScope();
    }

    void Visiting::visitFold(Fold *fold)
    {
        /* Code For Fold Goes Here */
        std::cout << "visitFold\n";

        if (fold->type_)
            fold->type_->accept(this);
        if (fold->expr_)
            fold->expr_->accept(this);
    }

    void Visiting::visitUnfold(Unfold *unfold)
    {
        /* Code For Unfold Goes Here */
        std::cout << "visitUnfold\n";

        if (unfold->type_)
            unfold->type_->accept(this);
        if (unfold->expr_)
            unfold->expr_->accept(this);
    }

    void Visiting::visitDotRecord(DotRecord *dot_record)
    {
        /* Code For DotRecord Goes Here */
        std::cout << "visitDotRecord\n";

        if (dot_record->expr_)
            dot_record->expr_->accept(this);
        std::cout << "Before Binding: " << printer.print(dot_record) << "\n";
        ObjectType objDot = contexts.top();
        contexts.pop();
        if(!objDot.contain(dot_record->stellaident_)){
            std::cout << "ERROR: Record hasn't field " << dot_record->stellaident_ << " on line: " << dot_record->line_number << "\n";
            exit(1);
        }
        contexts.push(objDot.get(dot_record->stellaident_));
//        visitStellaIdent(dot_record->stellaident_);
    }

    void Visiting::visitDotTuple(DotTuple *dot_tuple)
    {
        /* Code For DotTuple Goes Here */
        std::cout << "visitDotTuple\n";

        if (dot_tuple->expr_)
            dot_tuple->expr_->accept(this);

        if (contexts.top().typeTag != MyTypeTag::TupleTypeTag){
            std::cout << contexts.top().typeTag << "  ";
            std::cout << "ERROR: it's not tuple, on line: " << dot_tuple->line_number << " at position: " << dot_tuple->char_number << "\n";
            exit(1);
        }
        visitInteger(dot_tuple->integer_);
        if(contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: Should integer, on line: " << dot_tuple->line_number << "\n";
            exit(1);
        }
        int position = contexts.top().data;
        contexts.pop();
        if(position == -1) {
            std::cout << "SOMETHING WRONG\n";
            exit(1);
        }
        if(position > contexts.top().params.size()){
            std::cout << "ERROR: Out of range\n";
            exit(1);
        }
        int reversed = contexts.top().params.size() - position; // cause our params reversed
        ObjectType newObj = contexts.top().params[reversed];
        std::cout << "\n\n";
        contexts.pop();
        contexts.push(newObj);
    }

    void Visiting::visitConstTrue(ConstTrue *const_true)
    {
        /* Code For ConstTrue Goes Here */
        std::cout << "ConstTrue \n";
        contexts.push(ObjectType(MyTypeTag::BoolTypeTag));
    }

    void Visiting::visitConstFalse(ConstFalse *const_false)
    {
        /* Code For ConstFalse Goes Here */
        std::cout << "ConstFalse\n";
        contexts.push(ObjectType(MyTypeTag::BoolTypeTag));
    }

    void Visiting::visitConstUnit(ConstUnit *const_unit)
    {
        /* Code For ConstUnit Goes Here */
        std::cout << "visitConstUnit\n";
        contexts.push(MyTypeTag::UnitTypeTag);
    }

    void Visiting::visitConstInt(ConstInt *const_int)
    {
        /* Code For ConstInt Goes Here */
        std::cout << "ConstInt\n";

        visitInteger(const_int->integer_);
    }

    void Visiting::visitConstMemory(ConstMemory *const_memory)
    {
        /* Code For ConstMemory Goes Here */
        std::cout << "visitConstMemory\n";
        visitMemoryAddress(const_memory->memoryaddress_);
    }

    void Visiting::visitVar(Var *var)
    {
        /* Code For Var Goes Here */
        std::cout << "visitVar\n";
//        expected_type = contextIdent[var->stellaident_];

//        std::cout << "MyTag is the: " <<
        visitStellaIdent(var->stellaident_);


    }

    void Visiting::visitAPatternBinding(APatternBinding *a_pattern_binding)
    {
        /* Code For APatternBinding Goes Here */
        std::cout << "visitAPatternBinding\n";
        std::cout << printer.print(a_pattern_binding->pattern_) << "\n";
        // here we need first come to expr then pattern cause first we need take type of the pattern and then binding them
        if (a_pattern_binding->expr_)
            a_pattern_binding->expr_->accept(this);
        if (a_pattern_binding->pattern_)
            a_pattern_binding->pattern_->accept(this);

    }

    void Visiting::visitAMatchCase(AMatchCase *a_match_case)
    {
        /* Code For AMatchCase Goes Here */
        std::cout << "visitAMatchCase\n";
        increaseScope();

        ObjectType objSum = contexts.top();
        if (a_match_case->pattern_)
            a_match_case->pattern_->accept(this);

        int sizeBefore = contexts.size();
        if (a_match_case->expr_)
            a_match_case->expr_->accept(this);
        while (sizeBefore < contexts.size()){
            objSum.returns.push_back(contexts.top());
            contexts.pop();
        }
        // to assign return to sumtype I delete from context, and add upd objSum
        contexts.pop();
        contexts.push(objSum);
        std::cout << "AFTER EXPR: " << sizeBefore <<" "<<contexts.size()  << " " << contexts.top().typeTag<< "\n";
        decreaseScope();
    }

    void Visiting::visitNoTyping(NoTyping *no_typing)
    {
        /* Code For NoTyping Goes Here */
        std::cout << "visitNoTyping\n";
    }

    void Visiting::visitSomeTyping(SomeTyping *some_typing)
    {
        /* Code For SomeTyping Goes Here */
        std::cout << "visitSomeTyping\n";

        if (some_typing->type_)
            some_typing->type_->accept(this);
    }

    void Visiting::visitNoPatternData(NoPatternData *no_pattern_data)
    {
        /* Code For NoPatternData Goes Here */
        std::cout << "visitNoPatternData\n";
    }

    void Visiting::visitSomePatternData(SomePatternData *some_pattern_data)
    {
        /* Code For SomePatternData Goes Here */
        std::cout << "visitSomePatternData\n";

        if (some_pattern_data->pattern_)
            some_pattern_data->pattern_->accept(this);
    }

    void Visiting::visitNoExprData(NoExprData *no_expr_data)
    {
        /* Code For NoExprData Goes Here */
        std::cout << "visitNoExprData\n";
    }

    void Visiting::visitSomeExprData(SomeExprData *some_expr_data)
    {
        /* Code For SomeExprData Goes Here */
        std::cout << "visitSomeExprData\n";
        if (some_expr_data->expr_)
            some_expr_data->expr_->accept(this);
    }

    void Visiting::visitPatternVariant(PatternVariant *pattern_variant)
    {
        /* Code For PatternVariant Goes Here */
        std::cout << "visitPatternVariant\n";

        visitStellaIdent(pattern_variant->stellaident_);
        if (pattern_variant->patterndata_)
            pattern_variant->patterndata_->accept(this);
    }

    void Visiting::visitPatternInl(PatternInl *pattern_inl)
    {
        /* Code For PatternInl Goes Here */
        std::cout << "visitPatternInl\n";
        if(contexts.top().typeTag != MyTypeTag::SumTypeTag){
            std::cout << "ERROR: should be sum types, on line: " << pattern_inl->line_number << "\n";
            exit(1);
        }
        ObjectType objSumL = contexts.top();
        contexts.push(objSumL.params[0]);
        if (pattern_inl->pattern_)
        pattern_inl->pattern_->accept(this);
    }

    void Visiting::visitPatternInr(PatternInr *pattern_inr)
    {
        /* Code For PatternInr Goes Here */
        std::cout << "visitPatternInr\n";
        if(contexts.top().typeTag != MyTypeTag::SumTypeTag){
            std::cout << "ERROR: should be sum types, on line: " << pattern_inr->line_number << "\n";
            exit(1);
        }
        ObjectType objSumR = contexts.top();
        contexts.push(objSumR.params[1]);

        if (pattern_inr->pattern_)
        pattern_inr->pattern_->accept(this);
    }

    void Visiting::visitPatternTuple(PatternTuple *pattern_tuple)
    {
        /* Code For PatternTuple Goes Here */
        std::cout << "visitPatternTuple\n";
        if(contexts.top().typeTag != MyTypeTag::ListTypeTag){
            std::cout << "ERROR: should be list, on line: " << pattern_tuple->line_number << "\n";
            exit(1);
        }

        if (pattern_tuple->listpattern_)
            pattern_tuple->listpattern_->accept(this);
    }

    void Visiting::visitPatternRecord(PatternRecord *pattern_record)
    {
        /* Code For PatternRecord Goes Here */
        std::cout << "visitPatternRecord\n";
        if(contexts.top().typeTag != MyTypeTag::ListTypeTag){
            std::cout << "ERROR: should be list, on line: " << pattern_record->line_number << "\n";
            exit(1);
        }

        if (pattern_record->listlabelledpattern_)
            pattern_record->listlabelledpattern_->accept(this);
    }

    void Visiting::visitPatternList(PatternList *pattern_list)
    {
        /* Code For PatternList Goes Here */
        std::cout << "visitPatternList\n";
        if(contexts.top().typeTag != MyTypeTag::ListTypeTag){
            std::cout << "ERROR: should be list, on line: " << pattern_list->line_number << "\n";
            exit(1);
        }
        if (pattern_list->listpattern_)
            pattern_list->listpattern_->accept(this);
    }

    void Visiting::visitPatternCons(PatternCons *pattern_cons)
    {
        /* Code For PatternCons Goes Here */
        std::cout << "visitPatternCons\n";

        if (pattern_cons->pattern_1)
            pattern_cons->pattern_1->accept(this);
        if (pattern_cons->pattern_2)
            pattern_cons->pattern_2->accept(this);
    }

    void Visiting::visitPatternFalse(PatternFalse *pattern_false)
    {
        /* Code For PatternFalse Goes Here */
        std::cout << "visitPatternFalse\n";
        if(contexts.top().typeTag != MyTypeTag::BoolTypeTag){
            std::cout << "ERROR: The pattern isn't boolean, on line: " << pattern_false->line_number << "\n";
            exit(1);
        }
    }

    void Visiting::visitPatternTrue(PatternTrue *pattern_true)
    {
        /* Code For PatternTrue Goes Here */
        std::cout << "visitPatternTrue\n";
        if(contexts.top().typeTag != MyTypeTag::BoolTypeTag){
            std::cout << "ERROR: The pattern isn't boolean, on line: " << pattern_true->line_number << "\n";
            std::cout << contexts.top().typeTag << "\n";
            exit(1);
        }
    }

    void Visiting::visitPatternUnit(PatternUnit *pattern_unit)
    {
        /* Code For PatternUnit Goes Here */
        std::cout << "visitPatternUnit\n";
        if(contexts.top().typeTag != MyTypeTag::UnitTypeTag){
            std::cout << "ERROR: The pattern isn't boolean, on line: " << pattern_unit->line_number << "\n";
            exit(1);
        }
    }

    void Visiting::visitPatternInt(PatternInt *pattern_int)
    {
        /* Code For PatternInt Goes Here */
        std::cout << "visitPatternInt\n";
        if(contexts.top().typeTag != MyTypeTag::NatTypeTag){
            std::cout << "ERROR: The pattern isn't boolean, on line: " << pattern_int->line_number << "\n";
            exit(1);
        }

//        visitInteger(pattern_int->integer_);
    }

    void Visiting::visitPatternSucc(PatternSucc *pattern_succ)
    {
        /* Code For PatternSucc Goes Here */
        std::cout << "visitPatternSucc\n";

        if (pattern_succ->pattern_)
            pattern_succ->pattern_->accept(this);
    }

    void Visiting::visitPatternVar(PatternVar *pattern_var)
    {
        /* Code For PatternVar Goes Here */
        std::cout << "visitPatternVar\n";
//        std::cout << "ASSIGN: " << pattern_var->stellaident_ << " " << contexts.top().typeTag  << "\n";
        contextIdent[pattern_var->stellaident_].push(contexts.top());
        scopedContext.top().push_back(pattern_var->stellaident_);
        contexts.pop();
    }

    void Visiting::visitALabelledPattern(ALabelledPattern *a_labelled_pattern)
    {
        /* Code For ALabelledPattern Goes Here */
        std::cout << "visitALabelledPattern\n";

        visitStellaIdent(a_labelled_pattern->stellaident_);
        if (a_labelled_pattern->pattern_)
            a_labelled_pattern->pattern_->accept(this);
    }

    void Visiting::visitABinding(ABinding *a_binding)
    {
        /* Code For ABinding Goes Here */
        std::cout << "visitABinding\n";

//        visitStellaIdent(a_binding->stellaident_);
        if (a_binding->expr_)
            a_binding->expr_->accept(this);
        ObjectType recordField = contexts.top();
        contexts.pop();
        contexts.top().add(a_binding->stellaident_, recordField);
    }

    void Visiting::visitSequence(Sequence *sequence)
    {
        /* Code For Sequence Goes Here */
        std::cout << "visitSequence\n";
        if (sequence->expr_1)
            sequence->expr_1->accept(this);
        if (sequence->expr_2)
            sequence->expr_2->accept(this);
    }

    void Visiting::visitTypeFun(TypeFun *type_fun)
    {
        /* Code For TypeFun Goes Here */
        std::cout << "visitTypeFun\n";

        int sizeBefore = contexts.size();
        ObjectType obj = ObjectType(MyTypeTag::FunctionTypeTag);
        if (type_fun->listtype_)
            type_fun->listtype_->accept(this);
        while(contexts.size() > sizeBefore){
            obj.params.push_back(contexts.top());
            contexts.pop();
        }
        if (type_fun->type_)
            type_fun->type_->accept(this);

        obj.returns.push_back(contexts.top());
        contexts.pop();

        contexts.push(obj);
    }

    void Visiting::visitTypeRec(TypeRec *type_rec)
    {
        /* Code For TypeRec Goes Here */
        std::cout << "VisitRec\n";

        visitStellaIdent(type_rec->stellaident_);

        if(contexts.top().typeTag == MyTypeTag::UndefinedTag){
            std::cout << "ERROR: undefined variable on: " << type_rec->line_number << "\n";
            exit(1);

        }
        if (type_rec->type_)
            type_rec->type_->accept(this);
        contexts.pop();
    }

    void Visiting::visitTypeSum(TypeSum *type_sum)
    {
        /* Code For TypeSum Goes Here */
        std::cout << "visitTypeSum\n";
        Type *tmp = type_sum->clone();
        std::cout << printer.print(tmp) << " \n\n";
        std::cout << printer.print(type_sum) << "\n";
        std::cout << printer.print(type_sum->type_1) << "    " << printer.print(type_sum->type_2) << "\n\n";
        ObjectType objSum(MyTypeTag::SumTypeTag);
        if (type_sum->type_1)
            type_sum->type_1->accept(this);
        objSum.params.push_back(contexts.top());
        contexts.pop();
        if (type_sum->type_2)
            type_sum->type_2->accept(this);
        objSum.params.push_back(contexts.top());
        contexts.pop();
        contexts.push(objSum);
    }

    void Visiting::visitTypeTuple(TypeTuple *type_tuple)
    {
        /* Code For TypeTuple Goes Here */
        std::cout << "visitTypeTuple\n";
        int sizedBefore = contexts.size();
        ObjectType objTuple(MyTypeTag::TupleTypeTag);
        if (type_tuple->listtype_)
            type_tuple->listtype_->accept(this);
        while(contexts.size() > sizedBefore){
            objTuple.params.push_back(contexts.top());
            contexts.pop();
        }
        contexts.push(objTuple);
    }

void Visiting::visitTypeRecord(TypeRecord *type_record)
    {
        /* Code For TypeRecord Goes Here */
        std::cout << "visitTypeRecord\n";

        contexts.push(ObjectType(RecordsTypeTag));
        if (type_record->listrecordfieldtype_)
            type_record->listrecordfieldtype_->accept(this);
    }

    void Visiting::visitTypeVariant(TypeVariant *type_variant)
    {
        /* Code For TypeVariant Goes Here */
        std::cout << "visitTypeVariant\n";

        if (type_variant->listvariantfieldtype_)
            type_variant->listvariantfieldtype_->accept(this);
    }

    void Visiting::visitTypeList(TypeList *type_list)
    {
        /* Code For TypeList Goes Here */
        std::cout << "visitTypeList\n";
        ObjectType objList(MyTypeTag::ListTypeTag);
        if (type_list->type_)
            type_list->type_->accept(this);
        objList.params.push_back(contexts.top());
        contexts.pop(); // delete type which contain list
        contexts.push(objList); // add list
    }

    void Visiting::visitTypeBool(TypeBool *type_bool)
    {
        /* Code For TypeBool Goes Here */
        std::cout << "visitBool\n";
        printer.visitTypeBool(type_bool);
        std::cout << "AMAZING WOKR: " << printer.print(type_bool) << "\n\n";
        contexts.push(ObjectType(MyTypeTag::BoolTypeTag));

    }

    void Visiting::visitTypeNat(TypeNat *type_nat)
    {
        std::cout << "Visiting NAT: " << type_nat->line_number << "  "<< type_nat->char_number << "\n";
        /* Code For TypeNat Goes Here */
        contexts.push(ObjectType(MyTypeTag::NatTypeTag));
    }

    void Visiting::visitTypeUnit(TypeUnit *type_unit)
    {
        /* Code For TypeUnit Goes Here */
        std::cout << "visitTypeUnit\n";
//        type_unit->accept(this);
        contexts.push(ObjectType(MyTypeTag::UnitTypeTag));

    }
    void Visiting::visitTypeTop(TypeTop *type_top)
    {
        /* Code For TypeTop Goes Here */
        std::cout << "visitTypeTop\n";
    }

    void Visiting::visitTypeBottom(TypeBottom *type_bottom)
    {
        /* Code For TypeBottom Goes Here */
        std::cout << "visitTypeBottom\n";
    }

    void Visiting::visitTypeRef(TypeRef *type_ref)
    {
        /* Code For TypeRef Goes Here */
        std::cout << "visitTypeRef\n";
        if (type_ref->type_)
            type_ref->type_->accept(this);
        ObjectType objRef = ObjectType(MyTypeTag::ReferenceTypeTag);
        objRef.params.push_back(contexts.top());
        contexts.pop();
        contexts.push(objRef);
    }

    void Visiting::visitTypeVar(TypeVar *type_var)
    {
        /* Code For TypeVar Goes Here */
        std::cout << "visitTypeVar\n";
        visitStellaIdent(type_var->stellaident_);
        if(contexts.top().typeTag == MyTypeTag::UndefinedTag){
            std::cout << "ERROR: undefined variable on: " << type_var->line_number << "\n";
            exit(1);
        }

    }

    void Visiting::visitAVariantFieldType(AVariantFieldType *a_variant_field_type)
    {
        /* Code For AVariantFieldType Goes Here */
        std::cout << "visitAVariantFieldType\n";

        visitStellaIdent(a_variant_field_type->stellaident_);
        if (a_variant_field_type->optionaltyping_)
            a_variant_field_type->optionaltyping_->accept(this);
    }

    void Visiting::visitARecordFieldType(ARecordFieldType *a_record_field_type)
    {
        /* Code For ARecordFieldType Goes Here */
        std::cout << "visitARecordFieldType\n";

//        visitStellaIdent(a_record_field_type->stellaident_);
        if (a_record_field_type->type_)
            a_record_field_type->type_->accept(this);
        ObjectType field = contexts.top();
        contexts.pop();
        contexts.top().add(a_record_field_type->stellaident_, field);
    }

    void Visiting::visitATyping(ATyping *a_typing)
    {
        /* Code For ATyping Goes Here */
        std::cout << "visitATyping: \n";

        if (a_typing->expr_)
            a_typing->expr_->accept(this);
        if (a_typing->type_)
            a_typing->type_->accept(this);
    }

    void Visiting::visitListStellaIdent(ListStellaIdent *list_stella_ident)
    {
        std::cout << "visitListStellaIdent: \n";
        for (ListStellaIdent::iterator i = list_stella_ident->begin(); i != list_stella_ident->end(); ++i)
        {
            visitStellaIdent(*i);
        }
    }

    void Visiting::visitListExtensionName(ListExtensionName *list_extension_name)
    {
        std::cout << "visitListExtensionName: \n";
        for (ListExtensionName::iterator i = list_extension_name->begin(); i != list_extension_name->end(); ++i)
        {
            visitExtensionName(*i);

        }
    }

    void Visiting::visitListExtension(ListExtension *list_extension)
    {
        std::cout << "visitListExtension: \n";
        for (ListExtension::iterator i = list_extension->begin(); i != list_extension->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListDecl(ListDecl *list_decl)
    {
        std::cout << "visitListDecl: \n";
        for (ListDecl::iterator i = list_decl->begin(); i != list_decl->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListLocalDecl(ListLocalDecl *list_local_decl)
    {
        std::cout << "visitListLocalDecl: \n";
        for (ListLocalDecl::iterator i = list_local_decl->begin(); i != list_local_decl->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListAnnotation(ListAnnotation *list_annotation)
    {
        std::cout << "visitListAnnotation: \n";
        for (ListAnnotation::iterator i = list_annotation->begin(); i != list_annotation->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListParamDecl(ListParamDecl *list_param_decl)
    {
        std::cout << "visitListParamDecl: \n";
        for (ListParamDecl::iterator i = list_param_decl->begin(); i != list_param_decl->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListExpr(ListExpr *list_expr)
    {
        std::cout << "visitListExpr: \n";
        for (ListExpr::iterator i = list_expr->begin(); i != list_expr->end(); ++i)
        {
            std::cout << "----Before : \n" ;
            (*i)->accept(this);
            if(contexts.top().typeTag == MyTypeTag::UndefinedTag){
                std::cout << "ERROR: The variable undefined, on line: " << (*i)->line_number << " at position: " << (*i)->char_number << "\n";
                exit(1);
            }
        }
    }

    void Visiting::visitListMatchCase(ListMatchCase *list_match_case)
    {
        std::cout << "visitListMatchCase: \n";
        for (ListMatchCase::iterator i = list_match_case->begin(); i != list_match_case->end(); ++i)
        {
            std::cout << "NEW MATCH " << contexts.top().typeTag << "\n";
            (*i)->accept(this);
            std::cout << "END MATCH " << contexts.top().typeTag << "\n";
        }
    }

    void Visiting::visitListPattern(ListPattern *list_pattern)
    {
        std::cout << "visitListPattern: \n";
        for (ListPattern::iterator i = list_pattern->begin(); i != list_pattern->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListLabelledPattern(ListLabelledPattern *list_labelled_pattern)
    {
        std::cout << "visitListLabelledPattern: \n";
        for (ListLabelledPattern::iterator i = list_labelled_pattern->begin(); i != list_labelled_pattern->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListBinding(ListBinding *list_binding)
    {
        std::cout << "visitListBinding: \n";
        for (ListBinding::iterator i = list_binding->begin(); i != list_binding->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListType(ListType *list_type)
    {
        std::cout << "visitListType: \n";
        for (ListType::iterator i = list_type->begin(); i != list_type->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListPatternBinding(ListPatternBinding *list_pattern_binding)
    {
        std::cout << "visitListPatternBinding\n" ;
        for (ListPatternBinding::iterator i = list_pattern_binding->begin(); i != list_pattern_binding->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListVariantFieldType(ListVariantFieldType *list_variant_field_type)
    {
        std::cout << "visitListVariantFieldType: \n";
        for (ListVariantFieldType::iterator i = list_variant_field_type->begin(); i != list_variant_field_type->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitListRecordFieldType(ListRecordFieldType *list_record_field_type)
    {
        std::cout << "visitListRecordFieldType: \n";
        for (ListRecordFieldType::iterator i = list_record_field_type->begin(); i != list_record_field_type->end(); ++i)
        {
            (*i)->accept(this);
        }
    }

    void Visiting::visitInteger(Integer x)
    {
        /* Code for Integer Goes Here */
        contexts.push(ObjectType(MyTypeTag::NatTypeTag, x));
        std::cout << "visitInt: " << x << "\n";
    }

    void Visiting::visitChar(Char x)
    {
        /* Code for Char Goes Here */
        std::cout << "visitChar: " << x << "\n";
    }

    void Visiting::visitDouble(Double x)
    {
        /* Code for Double Goes Here */

        std::cout << "visitDouble: " << x << "\n";
    }

    void Visiting::visitString(String x)
    {
        /* Code for String Goes Here */
        std::cout << "visitString: " << x << "\n";
    }

    void Visiting::visitIdent(Ident x)
    {
        /* Code for Ident Goes Here */

        std::cout << "vistsIdent: " << x << "\n";
    }

    void Visiting::visitStellaIdent(StellaIdent x)
    {
        /* Code for StellaIdent Goes Here */
        std::cout << "Visiting StellaIdent: " << x << "\n";
        // when we come in this function I will put contextIdent[x] to context
        // By default contextIdent[x].typeTag will be Undefined, in case if we not meet this identifier it's mean get
        // default value. Other case It will work proper

        if(contextIdent[x].size() == 0){ // if size of stack is empty it's mean this ident not declared
            contexts.push(ObjectType(MyTypeTag::UndefinedTag));
        } else {
            contexts.push(contextIdent[x].top()); // add latest value which mean local value
        }
    }

    void Visiting::visitExtensionName(ExtensionName x)
    {
        std::cout << "visitExtensionName: " << x << "\n";
        /* Code for ExtensionName Goes Here */
    }

    void Visiting::visitMemoryAddress(MemoryAddress x)
    {
        /* Code for MemoryAddress Goes Here */
        std::cout << "visitMemoryAddress\n";
    }
}

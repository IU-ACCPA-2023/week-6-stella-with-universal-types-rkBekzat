//
// Created by bekzat on 27.03.23.
//
#include <stack>
#include <map>
#include <queue>
#include <vector>
#include <iostream>
#include "Enum.h"
#include <unordered_map>
#include "Stella/Absyn.H"
#include "Stella/Printer.H"
#include "ObjectType.h"


namespace Stella
{


class Visiting : public Visitor
    {
    private:
        Stella::PrintAbsyn printer = Stella::PrintAbsyn();
        std::unordered_map<StellaIdent, std::stack<ObjectType>> contextIdent = {}; // contextIdent[ident].top() - is the value of ident, it's local value. if stack is empty it's mean this ident not declared

        ObjectType expected_type ;
        std::stack<std::vector<StellaIdent>> scopedContext; // when entering to new scope I add the idents to vector, and when I go out I delete last element from stack
        std::stack<ObjectType> contexts;


        void increaseScope(){
            scopedContext.push(std::vector<StellaIdent>());
        }
        void decreaseScope(){
            std::vector<StellaIdent> removeIdent = scopedContext.top();
            for(int i = 0; i < removeIdent.size(); i++){
                StellaIdent name = removeIdent[i];
                contextIdent[name].pop();
            }
            scopedContext.pop();
        }

    public:

        ObjectType getArgs(int sizedBefore) {
            ObjectType result = ObjectType(MyTypeTag::FunctionTypeTag);
            while(sizedBefore < contexts.size()){
                result.params.push_back(contexts.top());
                contexts.pop();
            }
            return result;
        }

        bool subTypeRecord(ObjectType r, ObjectType sub){
            for(int i = 0; i < sub.records.size(); i++){
                bool find = false;
                for(int j = 0; j < r.records.size(); j++){
                    if(sub.records[i].first == r.records[j].first){
                        if(sub.records[i].second != r.records[j].second){
                            return false;
                        }
                        find = true;
{}                    }
                }
                if(!find){
                    return false;
                }
            }
            return true;
        }

        bool subTypeFunc(ObjectType f, ObjectType args){
            if(args.params.size() < f.params.size()){
                return false;
            }
            for(int i = 0; i < f.params.size(); i++){
                if(args.params[i].typeTag != f.params[i].typeTag){
                    return false;
                }
                if(args.params[i].typeTag == MyTypeTag::RecordsTypeTag){
                    if(!subTypeRecord(args.params[i], f.params[i])){
                        return false;
                    }
                }
                else if(args.params[i].typeTag == MyTypeTag::FunctionTypeTag){
                    if(!subTypeFunc(args.params[i], f.params[i])){
                        return false;
                    }
                    if(args.params[i].returns[0] != f.params[i].returns[0]){
                        return false;
                    }
                }
                else {
                    if(args.params[i] != f.params[i]){
                        return false;
                    }
                }
            }
            return true;
        }

    bool checkReturn(ObjectType actual, ObjectType expected){
        if(actual == expected){
            return true;
        }
        if(actual.typeTag == MyTypeTag::SumTypeTag){
            return checkReturn(actual.returns[0], expected) && checkReturn(actual.returns[1], expected);
        }
        return false;
    }

        void visitProgram(Program *p);
        void visitLanguageDecl(LanguageDecl *p);
        void visitExtension(Extension *p);
        void visitDecl(Decl *p);
        void visitLocalDecl(LocalDecl *p);
        void visitAnnotation(Annotation *p);
        void visitParamDecl(ParamDecl *p);
        void visitReturnType(ReturnType *p);
        void visitThrowType(ThrowType *p);
        void visitType(Type *p);
        void visitMatchCase(MatchCase *p);
        void visitOptionalTyping(OptionalTyping *p);
        void visitPatternData(PatternData *p);
        void visitExprData(ExprData *p);
        void visitPattern(Pattern *p);
        void visitLabelledPattern(LabelledPattern *p);
        void visitBinding(Binding *p);
        void visitExpr(Expr *p);
        void visitPatternBinding(PatternBinding *p);
        void visitVariantFieldType(VariantFieldType *p);
        void visitRecordFieldType(RecordFieldType *p);
        void visitTyping(Typing *p);
        void visitAProgram(AProgram *p);
        void visitLanguageCore(LanguageCore *p);
        void visitAnExtension(AnExtension *p);
        void visitDeclFun(DeclFun *p);
        void visitDeclTypeAlias(DeclTypeAlias *p);
        void visitDeclExceptionType(DeclExceptionType *p);
        void visitDeclExceptionVariant(DeclExceptionVariant *p);
        void visitALocalDecl(ALocalDecl *p);
        void visitInlineAnnotation(InlineAnnotation *p);
        void visitAParamDecl(AParamDecl *p);
        void visitNoReturnType(NoReturnType *p);
        void visitSomeReturnType(SomeReturnType *p);
        void visitNoThrowType(NoThrowType *p);
        void visitSomeThrowType(SomeThrowType *p);
        void visitAMatchCase(AMatchCase *p);
        void visitNoTyping(NoTyping *p);
        void visitSomeTyping(SomeTyping *p);
        void visitNoPatternData(NoPatternData *p);
        void visitSomePatternData(SomePatternData *p);
        void visitNoExprData(NoExprData *p);
        void visitSomeExprData(SomeExprData *p);
        void visitPatternVariant(PatternVariant *p);
        void visitPatternInl(PatternInl *p);
        void visitPatternInr(PatternInr *p);
        void visitPatternTuple(PatternTuple *p);
        void visitPatternRecord(PatternRecord *p);
        void visitPatternList(PatternList *p);
        void visitPatternCons(PatternCons *p);
        void visitPatternFalse(PatternFalse *p);
        void visitPatternTrue(PatternTrue *p);
        void visitPatternUnit(PatternUnit *p);
        void visitPatternInt(PatternInt *p);
        void visitPatternSucc(PatternSucc *p);
        void visitPatternVar(PatternVar *p);
        void visitALabelledPattern(ALabelledPattern *p);
        void visitABinding(ABinding *p);
        void visitSequence(Sequence *p);
        void visitAssign(Assign *p);
        void visitIf(If *p);
        void visitLet(Let *p);
        void visitLetRec(LetRec *p);
        void visitAPatternBinding(APatternBinding *p);
        void visitLessThan(LessThan *p);
        void visitLessThanOrEqual(LessThanOrEqual *p);
        void visitGreaterThan(GreaterThan *p);
        void visitGreaterThanOrEqual(GreaterThanOrEqual *p);
        void visitEqual(Equal *p);
        void visitNotEqual(NotEqual *p);
        void visitTypeAsc(TypeAsc *p);
        void visitTypeCast(TypeCast *p);
        void visitAbstraction(Abstraction *p);
        void visitVariant(Variant *p);
        void visitMatch(Match *p);
        void visitList(List *p);
        void visitAdd(Add *p);
        void visitSubtract(Subtract *p);
        void visitLogicOr(LogicOr *p);
        void visitMultiply(Multiply *p);
        void visitDivide(Divide *p);
        void visitLogicAnd(LogicAnd *p);
        void visitRef(Ref *p);
        void visitDeref(Deref *p);
        void visitApplication(Application *p);
        void visitDotRecord(DotRecord *p);
        void visitDotTuple(DotTuple *p);
        void visitTuple(Tuple *p);
        void visitRecord(Record *p);
        void visitConsList(ConsList *p);
        void visitHead(Head *p);
        void visitIsEmpty(IsEmpty *p);
        void visitTail(Tail *p);
        void visitPanic(Panic *p);
        void visitThrow(Throw *p);
        void visitTryCatch(TryCatch *p);
        void visitTryWith(TryWith *p);
        void visitInl(Inl *p);
        void visitInr(Inr *p);
        void visitSucc(Succ *p);
        void visitLogicNot(LogicNot *p);
        void visitPred(Pred *p);
        void visitIsZero(IsZero *p);
        void visitFix(Fix *p);
        void visitNatRec(NatRec *p);
        void visitFold(Fold *p);
        void visitUnfold(Unfold *p);
        void visitConstTrue(ConstTrue *p);
        void visitConstFalse(ConstFalse *p);
        void visitConstUnit(ConstUnit *p);
        void visitConstInt(ConstInt *p);
        void visitConstMemory(ConstMemory *p);
        void visitVar(Var *p);
        void visitTypeFun(TypeFun *p);
        void visitTypeRec(TypeRec *p);
        void visitTypeSum(TypeSum *p);
        void visitTypeTuple(TypeTuple *p);
        void visitTypeRecord(TypeRecord *p);
        void visitTypeVariant(TypeVariant *p);
        void visitTypeList(TypeList *p);
        void visitTypeBool(TypeBool *p);
        void visitTypeNat(TypeNat *p);
        void visitTypeUnit(TypeUnit *p);
        void visitTypeTop(TypeTop *p);
        void visitTypeBottom(TypeBottom *p);
        void visitTypeRef(TypeRef *p);
        void visitTypeVar(TypeVar *p);
        void visitAVariantFieldType(AVariantFieldType *p);
        void visitARecordFieldType(ARecordFieldType *p);
        void visitATyping(ATyping *p);
        void visitListStellaIdent(ListStellaIdent *p);
        void visitListExtensionName(ListExtensionName *p);
        void visitListExtension(ListExtension *p);
        void visitListDecl(ListDecl *p);
        void visitListLocalDecl(ListLocalDecl *p);
        void visitListAnnotation(ListAnnotation *p);
        void visitListParamDecl(ListParamDecl *p);
        void visitListType(ListType *p);
        void visitListMatchCase(ListMatchCase *p);
        void visitListPattern(ListPattern *p);
        void visitListLabelledPattern(ListLabelledPattern *p);
        void visitListBinding(ListBinding *p);
        void visitListExpr(ListExpr *p);
        void visitListPatternBinding(ListPatternBinding *p);
        void visitListVariantFieldType(ListVariantFieldType *p);
        void visitListRecordFieldType(ListRecordFieldType *p);

        void visitInteger(Integer x);
        void visitChar(Char x);
        void visitDouble(Double x);
        void visitString(String x);
        void visitIdent(Ident x);
        void visitStellaIdent(StellaIdent x);
        void visitExtensionName(ExtensionName x);
        void visitMemoryAddress(MemoryAddress x);
    };
}


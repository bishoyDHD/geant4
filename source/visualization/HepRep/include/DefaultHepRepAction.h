#ifndef DEFAULTHEPREPACTION_H
#define DEFAULTHEPREPACTION_H 1

#include "FreeHepTypes.h"

#include <string>

#include "HEPREP/HepRepAction.h"

/**
 *
 * @author M.Donszelmann
 * @version $Id: DefaultHepRepAction.h,v 1.6 2002-11-14 18:35:47 duns Exp $
 */
class DefaultHepRepAction : public virtual HEPREP::HepRepAction {

    private:
        std::string name;
        std::string expression;

    public:
        DefaultHepRepAction(std::string name, std::string expression);
        ~DefaultHepRepAction();

        std::string getName();
        std::string getExpression();
        HEPREP::HepRepAction* copy();
};

#endif

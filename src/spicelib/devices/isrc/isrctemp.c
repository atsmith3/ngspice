/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1985 Thomas L. Quarles
**********/

#include "ngspice/ngspice.h"
#include "ngspice/smpdefs.h"
#include "ngspice/cktdefs.h"
#include "isrcdefs.h"
#include "ngspice/sperror.h"
#include "ngspice/suffix.h"

#ifdef USE_CUSPICE
#include "ngspice/CUSPICE/CUSPICE.h"
#endif

/*ARGSUSED*/
int
ISRCtemp(GENmodel *inModel, CKTcircuit *ckt)
        /* Pre-process voltage source parameters
         */
{
    ISRCmodel *model = (ISRCmodel *) inModel;
    ISRCinstance *here;
    double radians;

    NG_IGNORE(ckt);

#ifdef USE_CUSPICE
    int i, j, status ;
#endif

    /*  loop through all the voltage source models */
    for( ; model != NULL; model = ISRCnextModel(model)) {

#ifdef USE_CUSPICE
    i = 0 ;
#endif

        /* loop through all the instances of the model */
        for (here = ISRCinstances(model); here != NULL ;
                here=ISRCnextInstance(here)) {

            if(here->ISRCacGiven && !here->ISRCacMGiven) {
                here->ISRCacMag = 1;
            }
            if(here->ISRCacGiven && !here->ISRCacPGiven) {
                here->ISRCacPhase = 0;
            }
            if(!here->ISRCdcGiven) {
                /* no DC value - either have a transient value, or none */
                if(here->ISRCfuncTGiven) {
                    SPfrontEnd->IFerrorf (ERR_WARNING,
                            "%s: no DC value, transient time 0 value used",
                            here->ISRCname);
                } else {
                    SPfrontEnd->IFerrorf (ERR_WARNING,
                            "%s: has no value, DC 0 assumed",
                            here->ISRCname);
                }
            }
            if(!here->ISRCmGiven)
                here->ISRCmValue = 1;
            radians = here->ISRCacPhase * M_PI / 180.0;
            here->ISRCacReal = here->ISRCacMag * cos(radians);
            here->ISRCacImag = here->ISRCacMag * sin(radians);

#ifdef USE_CUSPICE
            for (j = 0 ; j < here->n_coeffs ; j++)
            {
                model->ISRCparamCPU.ISRCcoeffsArrayHost [i] [j] = here->ISRCcoeffs [j] ;
            }

            model->ISRCparamCPU.ISRCdcvalueArray[i] = here->ISRCdcValue ;
            model->ISRCparamCPU.ISRCdcGivenArray[i] = here->ISRCdcGiven ;
            model->ISRCparamCPU.ISRCfunctionTypeArray[i] = here->ISRCfunctionType ;
            model->ISRCparamCPU.ISRCfunctionOrderArray[i] = here->ISRCfunctionOrder ;

            i++ ;
#endif

        }

#ifdef USE_CUSPICE
        status = cuISRCtemp ((GENmodel *)model) ;
        if (status != 0)
            return (E_NOMEM) ;
#endif

    }

    return(OK);
}

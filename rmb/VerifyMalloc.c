//
//  VerifyMalloc.c
//  projectoRCI
//
//  Created by João Valente on 08/03/2017.
//  Copyright © 2017 João Valente. All rights reserved.
//

#include "Defs.h"

void VerifyMalloc(void *pointer)
{
    if(pointer == NULL)
    {
        printf("Error allocating memory");
        exit(0);
    }
}

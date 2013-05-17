
// vi: tabstop=4:expandtab

#ifndef MACI_SHAPE_FUNCTION_HH_INCLUDED
#define MACI_SHAPE_FUNCTION_HH_INCLUDED 1

/// First order spline function
/// @param[in]  N   number of corners of the element
/// @param[in]  i   corner index
/// @param[in]  Xi  local coordinates
template<typename Float>
Float ShapeFunction(int N, int i, const Float Xi[])
{
#if 2 == DIM
    switch(N) 
    {
    case 3:
        switch(i) 
        {
        case 0: return 1.0 - Xi[0] - Xi[1];
        case 1: return       Xi[0];
        case 2: return               Xi[1];
        }
    case 4:
        switch(i) 
        {
        case 0: return (1.0 - Xi[0])*(1.0 - Xi[1]);
        case 1: return        Xi[0] *(1.0 - Xi[1]);
        case 2: return        Xi[0] *       Xi[1];
        case 3: return (1.0 - Xi[0])*       Xi[1];
        }
    }
#endif
#if 3 == DIM
        switch(N)
        {
        case 4:
            switch(i) 
            {
            case 0: return 1.0 - Xi[0] - Xi[1] - Xi[2];
            case 1: return Xi[0];
            case 2: return Xi[1];
            case 3: return Xi[2];
            }
        case 5:
            switch(i) 
            {
            case 0 :
                if (Xi[0] > Xi[1])
                    return (1.0 - Xi[0])*(1.0 - Xi[1]) + Xi[2]*(Xi[1] - 1.0);
                else
                    return (1.0 - Xi[0])*(1.0 - Xi[1]) + Xi[2]*(Xi[0] - 1.0);
            case 1 :
                if (Xi[0] > Xi[1])
                    return Xi[0]*(1.0 - Xi[1]) - Xi[2] * Xi[1];
                else
                    return Xi[0]*(1.0 - Xi[1]) - Xi[2] * Xi[0];
            case 2 :
                if (Xi[0] > Xi[1])
                    return Xi[0] * Xi[1] + Xi[2] * Xi[1];
                else
                    return Xi[0] * Xi[1] + Xi[2] * Xi[0];
            case 3 :
                if (Xi[0] > Xi[1])
                    return (1.0 - Xi[0]) * Xi[1] - Xi[2] * Xi[1];
                else
                    return (1.0 - Xi[0]) * Xi[1] - Xi[2] * Xi[0];
            case 4: return Xi[2];
            }
        case 6:
            switch(i) 
            {
            case 0: return (1.0 - Xi[0] - Xi[1])*(1.0 - Xi[2]);
            case 1: return Xi[0]*(1.0 - Xi[2]);
            case 2: return Xi[1]*(1.0 - Xi[2]);
            case 3: return (1.0 - Xi[0] - Xi[1]) * Xi[2];
            case 4: return Xi[0] * Xi[2];
            case 5: return Xi[1] * Xi[2];
            }
        case 8:
            switch(i) 
            {
            case 0: return (1.0 - Xi[0])*(1.0 - Xi[1])*(1.0 - Xi[2]);
            case 1: return (Xi[0])*(1.0 - Xi[1])*(1.0 - Xi[2]);
            case 2: return (Xi[0])*Xi[1]*(1.0 - Xi[2]);
            case 3: return (1.0 - Xi[0])*(Xi[1])*(1.0 - Xi[2]);
            case 4: return (1.0 - Xi[0])*(1.0 - Xi[1])*Xi[2];
            case 5: return Xi[0]*(1.0 - Xi[1])*Xi[2];
            case 6: return Xi[0]*Xi[1]*Xi[2];
            case 7: return (1.0 - Xi[0])*Xi[1]*Xi[2];
            }
        }
#endif
    
    THROW(BadArgException, "One of the arguments (N = %d, i = %d) are invalid.", N, i);
}

#endif


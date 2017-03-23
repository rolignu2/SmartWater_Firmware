
/***
 * @description : objeto instalador en localidad de memoria
 * @version : 1.0.1
 * @author : Rolando Arriaza
***/


struct ObjSetup {
        int                 version;
        char                webservices[50];
        char                web_base[45];
        char                error[45];
        char                base_error[45];
        int                 activate ;
        int                 sleep ;
        int                 tick;
        char                variables[650];
        char                control_service[50];
        char                control_base[45];
        char                war[10];
};




//clase por defecto de configuracion
class DefConfig
{

    public :

          static  ObjSetup get_DefConf()
            {
               ObjSetup s =  {
                                1 ,                             //version
                                "hook-response/Ebase" ,         //hook webservices
                                "Ebase" ,                       //WS communicator
                                "hook-response/Aerror" ,        // hook errror
                                "Aerror"  ,                     // WS error communicator
                                1 ,                             // particle active
                                0 ,                             // sleep mode
                                2000,                           // tick
                                "holacomoestas",
                                "hook-response/Econtrol",       //web services control
                                "Econtrol",                     //hook control
                                "war"                           // war data set http client
                        };

                return s;
            }



};

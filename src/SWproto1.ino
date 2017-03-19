/***
 * @SmartWater
 * @version                 1.2.0 [RC 1.2]
 * @author                  Rolando Arriaza --> rolignu90
 * @description             Core del firmware smartwater

 * Descripcion de los servicios
 *
 *  EBASE       = envio de data al web services : se encarga de establecer la data y la logica dentro del WS
 *  AERROR      = envio de errores de parte del dispositivo al webservices
 *  ECONTROL    = Gestor de control del dispositivos , en el cual se ve estados en tiempo real
 *  WAR         = de parte del cliente o HTTP envia comandos de configuracion
****/



#include <SmartObject.h>
#include <ParseVariable.cpp>


/*****************************  SMART WATER INICIO E INSTANCIA DE LA CLASE  *******************/
/***
 * @description : Esta clase fue desarrollada con el fin de hacer una ejecucion en segundo plano
 *                a pesar de que esta la de primer plano se ejecuta asincronamente.
 * @version : 1.0
 * @author : Rolando Arriaza
***/
class Task
{
    public :


              Task() :  timer(this->tick , &Task::task_process , *this) {}
               ~Task() { this->task_stop(); }
               void task_process(void);
               void task_start()  { timer.start();  }
               void task_stop(){ timer.stop(); }
               void new_period(int t){
                   this->tick = t;
                   timer.changePeriod(this->tick);
               }
               void reset_task() { timer.reset();   }

    private:
             int tick       = 10000;
             Timer timer;
};




class Swater : public Task , public SmartVariables
{

    public:

             /**
             * @description  Constructor de la clase hace referencia al void setup ()
             *               este constructor carga todas las funciones que se desean utilizar
             * @version 1.0
             * @author Rolando Arriaza
            **/
            Swater() : timer(this->tick , &Swater::loop , *this) , Task() {


                ObjSetup  setup  = this->get_PrimaryConf();

                //configuracion por defecto
                ObDefault_ = DefConfig::get_DefConf();

                if(setup.version  != 0){

                    Swater::set_PrimaryConf(ObDefault_);
                    delay(2000);
                }


                if(this->tick != ObDefault_.tick )
                {
                     this->tick =  ObDefault_.tick ;
                }


            }



             /**
             * @description  inicia los objetos despues del constructor
             * @version 1.0
             * @author Rolando Arriaza
             * @param Tstart boolean , verifica si se ejecuta el timer
            **/
            void init(boolean Tstart)   {


                //testmode se utiliza la variable callback con el resultado dentro de json_compose
                if(test_mode){
                    Particle.variable("callback" , json_compose);
                }


                //regresa la configuraciond e memoria a modo default
                if(default_mode)
                {
                    set_PrimaryConf(ObDefault_);
                }


                 ObjSetup  setup  = this->get_PrimaryConf();

                //nombre del webhook donde esta alojado el WS
                 web_service = String(setup.webservices);

                 //web base
                 web_base = String(setup.web_base);

                 //web services error
                 web_service_error = String(setup.error);

                 //hook name error
                 web_base_error = String(setup.base_error);

                 //control_service
                 control_service = String(setup.control_service);

                 //control del hook
                 web_control     = String(setup.control_base);

                 //war function services
                 web_config      = String(setup.war);

                //funcion de particle en la cual se podra enviar las configuraciones WAR
                Particle.function(web_config, &Swater::set_config , this);


                //llamada hacia el webhook , funcion por defecto Gettrigget
                Particle.subscribe(this->web_service,  &Swater::GetTrigger  , this);


                //llamada hace el webhook error data handler
                Particle.subscribe(this->web_service_error , &Swater::GetErrorHandle , this);


                //llamada o suscripcion al servidor de control
                Particle.subscribe(this->control_service, &Swater::GetControlHandle , this );


                 //verifica el status del firmware ...
                 this->FirmStatus();
                 //llamada de wg
                // watch_dog = new ApplicationWatchdog(60000 , System.reset );


                //instanciamos el timer dentro de init
                 if(Tstart)
                    timer.start();


                 if(activate_task)
                    this->task_start();

                // if(this->activate_task)
                 //   task.start();

            }



             /**
             * @description  destructor de la clase
             * @version 1.0
             * @author Rolando Arriaza
            **/
            ~Swater(){}



             /**
             * @description  desarrolla un loop controlado
             * @version 1.0
             * @author Rolando Arriaza
            **/
            void loop(void);




            /**
             * @description  verifica el estado del firmware
             * @version 1.0
             * @author Rolando Arriaza
            **/
            int FirmStatus();


            /**
             * @description  Proceso en el cual obtiene los valores que enviara el photon
             * @version 1.0
             * @author Rolando Arriaza
            **/
            String get_process();



            /******************************FUNCIONES VARIAS**************************************/


            /**
             * @description  activa el modo test esta ligada a la variable particle callback
             * @version 1.0
             * @author Rolando Arriaza
             * @params boolean test_mode
            **/
            void start_testMode(boolean t){ test_mode = t; }




             /**
             * @description  activa la configuracion general en modo defualt
             * @version 1.0
             * @author Rolando Arriaza
             * @params boolean d = true , borra la configuracion anterior
            **/
            void start_defaultMode(boolean d) { default_mode = d; }



            /**
             * @description establecemos el periodo de la ejecucion loop
             * @version 1.0
             * @author Rolando Arriaza
             * @params int t = tiempo en milisegundos
            **/
            void set_period(int t) {
                tick = t;
                timer.changePeriod(tick);
            }


            /**
             * @description  se establece el tiempo tick pero este no afecta el periodo de ejecucion
             * @version 1.0
             * @author Rolando Arriaza
             * @params int t = tick en milisegundos
            **/

            void set_tick (int t) { tick = t; }




            /**
             * @description detiene el ciclo de ejecucion del loop
             * @version 1.0
             * @author Rolando Arriaza
            **/

            void stop_timer(){ timer.stop(); }


             /**
             * @description establece la configuracion del sistema en el WAR
             * @version 1.0
             * @author Rolando Arriaza
            **/
            int set_config(String command);




           /**
             * @description clase de la tarea asincrona
             * @version 1.0
             * @author Rolando Arriaza
            **/
           // Task task;



            /**
             * Funciones para la configuracion inicial de los servidores
             * utilizando la memoria EEPROM por defecto.
            ***/


            /**
             * @description ejecuta una nueva escritura de configuracion en la EEPROM
             * @version 1.1
             * @author Rolando Arriaza
            **/
            int set_PrimaryConf(ObjSetup o) {
                int size = (EEPROM.length() - 1);
                if(size > 2000)
                {
                    EEPROM.put(this->addr_  , o);

                }
                else
                {
                    String s = String("[Error, Memoria EEPROM insuficiente para ejecutar la configuracion , Error Critico ");
                    this->ExecuteErrorHandle (s);
                    return 0;
                }


                return 1;
            }



              /**
             * @description obtiene la configuracion desde la localidad en la EEPROM
             * @version 1.1
             * @author Rolando Arriaza
            **/
            ObjSetup get_PrimaryConf()
            {
                 int size = (EEPROM.length() - 1);
                 ObjSetup o ;

                 if(size > 2000)
                    EEPROM.get(this->addr_ , o);

                 return o;

            }


             /**
             * @description Avtiva la tarea asincrona
             * @version 1.1
             * @author Rolando Arriaza
            **/
            void start_task(boolean t) { activate_task = t; }


            //web hook execute
            void ExecuteHook(String process) {  Particle.publish(web_base, process, PRIVATE);  }


            //Split function command
            String SplitCommand(String data, char separator, int index)
            {
                int found = 0;
                int strIndex[] = { 0, -1 };
                int maxIndex = data.length() - 1;

                 for (int i = 0; i <= maxIndex && found <= index; i++) {
                    if (data.charAt(i) == separator || i == maxIndex) {
                        found++;
                        strIndex[0] = strIndex[1] + 1;
                        strIndex[1] = (i == maxIndex) ? i+1 : i;
                    }
                }

                return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
            }


    private:



        /***************VARIABLES O CONSTANTES PRIVADAS **********/

        //ejecucion de watchdog
        // ApplicationWatchdog * watch_dog ;

         // nombre del hook
         String  web_service;

         // hook base
         String  web_base ;

         //hook control

         String  web_control ;


         //web services error callback
         String  web_service_error ;

         //hook error callback
         String web_base_error;

         //control service
         String control_service;


         // webserver war
         String web_config                  = "war";


         //tiempo de retardo en milisegundos
         int    tick                        = 2000;


         //creando un loop por medio de timer
         Timer timer;



         //compositor de json
         char  json_compose[700];


         //bufer estatico json
        // StaticJsonBuffer<600>      jsonBuffer;



         //test mode
         boolean test_mode                  = false;

         boolean default_mode               = false;

         boolean activate_task              = false;


         //variable de iteracion

         int iteration                      = 0;



         /*************FUNCIONES PRIVADAS ************************/

         //webhook trigger
         void GetTrigger (const char *event, const char *data) {};



         //Error handler para errores detectados en el hook
         void GetErrorHandle(const char *event, const char *data) {};


         //Control del dispositivo Handle de accion
         void GetControlHandle(const char *event, const char *data){};


         //Dispositivo de ejecucion de control
         void ExecuteControlDevice(bool status)
         {
             switch(status)
             {
                 case true :
                    break;
                 case false :
                    return;
             }
         }


         //web hook error execute
         void ExecuteErrorHandle(String error) { Particle.publish(web_base_error, error, PRIVATE);  }





    protected:


         //direccion inicial de la memoria EEPROM
         int addr_      = 1;

         //configuracion por defecto
         ObjSetup ObDefault_;


};



/**
 * Algoritmo que verifica el estado del firmware
 * esto ayuda a verificar si existe problemas o si
 * esta fallando la conexion en un tiempo T dado
***/
int Swater::FirmStatus()
{
    if(WiFi.connecting())
    {
        this->FirmStatus();
        delay(5000);
       // return 0;
    }

    if(WiFi.ready())
    {

        if (!Particle.connected()){
            Particle.connect();
            delay(5000);
            this->FirmStatus();
           //return 0;
        }

    }
    else
        return 0;

    return 1;
}






/***
 * algoritmo de configuracion al momento que se ejecuta el WAR
 * la ejecucion de este war se viene dado desde la aplicacion web
 * generando asi el servicio respectivo , una vez ejecutado
 * se busca el comando y se ejecuta la funcion asociada
***/
int Swater::set_config(String command){

   //sobre escritura de la memoria eeprom
   bool ovewrite = false;

   //patron asignado al momento de hacer el split
   char pattern = ';';

   //comandos a analizar
   String cmd   = SplitCommand(command , pattern , 0);
   String exec  = SplitCommand(command , pattern ,  1);

   ObjSetup object = ObDefault_;

   //si el comando devuelve nada o vacio entonces generar un error
   if(cmd == "")
   {
       this->ExecuteErrorHandle ( String("[Error, El comando : " +  command +  " no se parametrizo de forma correcta [devuelve nulo] , Error Critico "));
       return -1;
   }


   if(cmd == "PARTICLE")
   {

      if(exec == "ACTIVATE" && Particle.connected() == false){
             Particle.connect();
             return 1;
      }
      else if(exec == "RESET" ){
           System.reset();
           return 1;
      }
      else if(exec == "SAFE_MODE"){
          System.enterSafeMode();
          return 1;
      }

   }
   else if(cmd == "VARIABLES")
   {
       strcpy(object.variables, exec.c_str());
       ovewrite = true;
       ObDefault_ = object;
   }

      //se activo la sobre escitura
      if(ovewrite){
          this->set_PrimaryConf(object);
          this->init(false);
      }

     return 1;
}



/**
 * es un algoritmo de tipo asincrono que se ejecuta independientemente
 * del loop actual , este proceso dura segun la configuracion del usuario
 * ojo !! como es asincrono si existen resultados al llamar en la funcion
 * loop y como sus timers no estan sincronizados puede obtener o establecer
 * datos no deseados.
***/
void Task::task_process(void){



}


/**
 * loop es el ciclo principal, todo codigo que se desea ejecutar en un tiempo T
 * debera ir aca en esta funcion. no esta ligada a task. teoricamente loop seguira ejecutandoce
 * de forma independiente.
***/
void Swater::loop(void){


     // codigo ejemplo .... no borrar
     //envio de datos por medio de cloud particle ...
     //sprintf(json_compose, "memoria eeprom  %u  numero de iteracion = %u " , memory_size , iteration );

     // ObjSetup o = get_PrimaryConf();
     // sprintf(json_compose, " eeprom lectura exitosa  %s  y T %u" ,  o.webservices, o.tick );


      //primera condicion en el loop , verificar el firmware
      if(this->FirmStatus() == 0 )
            return;

      //obtenemos los datos del proceso
      String process = this->get_process();

      //ejecutamos el hook del webservices
      ExecuteHook(process);

}




String Swater::get_process()
{

  char jsonString[] = "{\"query\":{\"count\":1,\"created\":\"2012-08-04T14:46:03Z\",\"lang\":\"en-US\",\"results\":{\"item\":{\"title\":\"valida de verga\"}}}}";


  //char* value = this->parseJson(jsonString);


   // return root;
   //return String(value);
  // return String(jsonString);

    //ObjSetup o = this->get_PrimaryConf();

    //return String(o.variables);
    ObjSetup o = ObDefault_ ;
    //char data[650];
    //strcpy( data, (const char*)o.variables);

    String data = "";
    /*const char * values = (const char *) o.variables;
    for(int i = 0 ; i < sizeof(values) ; i++){
        data += String(values[i]);
    }*/
    for(int i = 0 ; i < sizeof(o.variables)-1 ; i++){
      if(o.variables[i] != '\0' )
          data += String(o.variables[i]);
      else
          break;
    }


    return String( data);

 }



Swater smartwater;

void setup(){
    smartwater.set_period(50*100);
   // smartwater.start_task(true);
    smartwater.start_testMode(true);
    smartwater.init(true) ;
}

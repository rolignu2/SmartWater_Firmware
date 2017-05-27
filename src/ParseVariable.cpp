#include <aJSON.h>

class SmartVariables {

  public :

      /**
         @author Rolando Arriaza
         @version 1.0.0
         @description devuelve un valor de la configuracion json
         @param char* jsonString cadena que contiene el json {}
         @param char* type , tipo de dato a buscar en el json { "version " : 1 }
                      donde version vamos a buscar
         @return char* el valor de la llave encontrada version:1 ->devuelve 1
      */
      char * ConfJson(char * jsonString , char * type){

          aJsonObject* root = aJson.parse(jsonString);

         if(root == NULL)
              return NULL;

          aJsonObject* query = aJson.getObjectItem(root, type);

          return query->valuestring;
      }


      char** parseVars(char* jsonString , char ** patterns ){

          this->last_json = jsonString;

          return NULL;
      }

      char * get_lastJson(){ return this->last_json; }

    private :

         char * last_json;

};

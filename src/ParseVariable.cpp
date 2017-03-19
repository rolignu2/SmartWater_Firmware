#include <aJSON.h>

class SmartVariables {

  public :

      char * parseJson(char *jsonString){
          char* value;

          aJsonObject* root = aJson.parse(jsonString);

          if (root != NULL) {
              //Serial.println("Parsed successfully 1 " );
              aJsonObject* query = aJson.getObjectItem(root, "query");

              if (query != NULL) {
                  //Serial.println("Parsed successfully 2 " );
                  aJsonObject* results = aJson.getObjectItem(query, "results");

                  if (results != NULL) {
                      //Serial.println("Parsed successfully 3 " );
                      aJsonObject* item = aJson.getObjectItem(results, "item");

                      if (item != NULL) {
                          //Serial.println("Parsed successfully 4 " );
                          aJsonObject* title = aJson.getObjectItem(item, "title");

                          if (title != NULL) {
                              //Serial.println("Parsed successfully 5 " );
                              value = title->valuestring;
                          }
                      }
                  }
              }
          }

          if (value) {
              return value;
          } else {
              return NULL;
          }
      }

  private :

};

 
// Servidor basado en https://www.gnu.org/software/libmicrohttpd/tutorial.html#Hello-browser-example

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>

#define PORT 8888

//GNU libmicrohttpd llamará a la función que vamos a escribir ahora cada vez que ingrese una solicitud apropiada.

int answer_to_connection (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls)
{
  const char *page  = "<html><body>Hola Servidor!!</body></html>";
  //Para obtener dicha respuesta, entregamos nuestros datos (la cadena de respuesta) y su tamaño a la función 
  //MHD_create_response_from_buffer. Los dos últimos parámetros básicamente le dicen a MHD que no queremos que 
  //elimine los datos del mensaje para nosotros cuando se ha enviado y que tampoco es necesario realizar una copia 
  //interna porque la cadena constante no cambiará de todos modos.

  struct MHD_Response *response;
  int ret;

  response = MHD_create_response_from_buffer (strlen (page),
                                            (void*) page, MHD_RESPMEM_PERSISTENT);
	/*
	Ahora que la respuesta se ha unido, está lista para la entrega y se puede poner en cola para enviar. 
	Esto se hace pasándolo a otra función libmicrohttpd de GNU. Como todo nuestro trabajo se realizó en 
	el ámbito de una función, el destinatario es sin duda el asociado con la conexión de la variable local y, 
	en consecuencia, esta variable se asigna a la función de cola. Cada respuesta HTTP va acompañada de 
	un código de estado, aquí "OK", para que el cliente sepa que esta respuesta es el resultado previsto 
	de su solicitud y no debido a algún error o mal funcionamiento.
	*/										

  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);

  return ret;
}

//Main

int main ()
{
  struct MHD_Daemon *daemon;

  daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                             &answer_to_connection, NULL, MHD_OPTION_END);
  if (NULL == daemon) return 1;

/*

El primer parámetro es uno de los tres modos de operación posibles. Aquí queremos que el demonio se ejecute en un hilo separado 
y que administre todas las conexiones entrantes en el mismo hilo. Esto significa que mientras se produce la respuesta para una 
conexión, las otras conexiones se pondrán en espera. En este ejemplo, donde la respuesta ya se conoce y, por lo tanto, la solicitud 
se atiende rápidamente, esto no plantea ningún problema.Permitiremos que todos los clientes se conecten independientemente de su 
nombre o ubicación, por lo tanto, no los verificamos en la conexión y establecemos el tercer y cuarto parámetro en NULL.El parámetro
 cinco es la dirección de la función que queremos que se llame siempre que se establezca una nueva conexión. 
 
Nuestra answer_to_connection sabe mejor lo que el cliente quiere y no necesita información adicional (que podría pasarse a través
del siguiente parámetro), por lo que el siguiente (sexto) parámetro es NULL. Del mismo modo, no necesitamos pasar opciones 
adicionales al demonio, así que solo escribimos el MHD_OPTION_END como último parámetro. A medida que el daemon del servidor se 
ejecuta en segundo plano en su propio hilo, el flujo de ejecución en nuestra función principal se detendrá justo después de la 
llamada. Debido a esto, debemos retrasar el flujo de ejecución en el hilo principal o de lo contrario el programa terminará 
prematuramente. Dejamos que se detenga de manera amigable con el tiempo de procesamiento esperando que se presione la tecla Intro. 
Al final, detenemos al demonio para que pueda realizar sus tareas de limpieza.
*/


getchar ();

  MHD_stop_daemon (daemon);
  return 0;
}




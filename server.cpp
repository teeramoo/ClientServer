 #include<stdio.h>
   #include<string.h>
   #include<sys/socket.h>
   #include<arpa/inet.h>
   #include<unistd.h>
   #include<iostream>
   #include<fstream>
   #include<errno.h>
#include <boost/filesystem.hpp>
#include <boost/lambda/bind.hpp>
#include <chrono>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <string>
#include <iomanip>
#include <fcntl.h>

using namespace std;


int send_image(int socket, int ReceivedImage){

//   FILE *picture;
   int size, read_size, stat, packet_index;
   char send_buffer[100000], read_buffer[256];
   packet_index = 1;

 using namespace boost::filesystem;
    using namespace boost::lambda;
      using namespace std::chrono;

string imfolderpath = "/home/teeramoo/Desktop/ORB-slam-script/TransferImagesOverNetwork/Server/Images/";
    path the_path( imfolderpath );
 int cnt = std::count_if(
        directory_iterator(the_path),
        directory_iterator(),
        static_cast<bool(*)(const path&)>(is_regular_file) );

	cout << "Images to be sent : " << cnt << endl;

	 write(socket, (void *) &cnt, sizeof(int));
//    send(socket, (void *) &cnt, sizeof(int),MSG_CONFIRM);

for(int i=0;i<cnt;i++)
{
  cout << "Sending image number : " << i+1 << endl ;
  std::ostringstream ss;
  ss << std::setw(8) << std::setfill('0') << i+1 ;
  std::string s(ss.str());
	string imagePath = imfolderpath + s +".jpg";
	char *impath = new char[imagePath.length()];
//	char *impath = new char[imagePath.length()+1];
	std::strcpy(impath, imagePath.c_str());
cout << imagePath << endl;
FILE *picture;
   picture = fopen(impath, "r");
   printf("Getting Picture Size\n");

   if(picture == NULL) {
        printf("Error Opening Image File\n");
break;
}
cout << "Picture loaded. Sending...." << endl;
//   cout << "running fseek(picture, 0, SEEK_END); " << endl;
   fseek(picture, 0, SEEK_END);
//      cout << "size = ftell(picture); " << endl;
   size = ftell(picture);
//   cout << "fseek(picture, 0, SEEK_SET); " << endl;
   fseek(picture, 0, SEEK_SET);
   printf("Total Picture size: %i\n",size);

   //Send Picture Size
   printf("Sending Picture Size\n");
   write(socket, (void *)&size, sizeof(int));
//send(socket, (void *) &cnt, sizeof(int),MSG_CONFIRM);
   //Send Picture as Byte Array
   printf("Sending Picture as Byte Array\n");

   do { //Read while we get errors that are due to signals.
      stat=read(socket, &read_buffer , 255);
      printf("Bytes read: %i\n",stat);
   } while (stat < 0);

   printf("Received data in socket\n");
   printf("Socket data: %c\n", read_buffer);

   while(!feof(picture)) {
   //while(packet_index = 1){
      //Read from the file into our send buffer
      read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

      //Send data through our socket
      do{
//        stat = send(socket, (void *) &cnt, sizeof(int),MSG_CONFIRM);
        stat = write(socket, send_buffer, read_size);
      }while (stat < 0);

      printf("Packet Number: %i\n",packet_index);
      printf("Packet Size Sent: %i\n",read_size);
      printf(" \n");
      printf(" \n");


      packet_index++;

      //Zero out our send buffer
      bzero(send_buffer, sizeof(send_buffer));
     }
     fclose(picture);
     usleep(100000); // change to recv message
}

    }

    int main(int argc , char *argv[])
    {
      int socket_desc , new_socket , c, read_size,buffer = 0, ReceivedImage = 0;
      struct sockaddr_in server , client;
      char *readin;

      server.sin_family = AF_INET;
      //server.sin_addr.s_addr = INADDR_ANY;
      server.sin_addr.s_addr = inet_addr("127.0.0.1");
      server.sin_port = htons( 8889 );

      bool isSocketCreated = false;
      bool isBind = false;
      bool isAccept = false;
      //Create socket

while(1)
{
  // Create socket
  if(!isSocketCreated)
  {
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    fcntl(socket_desc, F_SETFL, O_NONBLOCK);
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    setsockopt(socket_desc, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    while (socket_desc == -1)
    {
      cout << "Could not create socket. Retrying..." << endl;
      sleep(1);
    }
    isSocketCreated = true;
  }

  //Bind socket
  if(isBind)
  {
    cout << "socket is already bind." << endl;
  }
  else
  {

      if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
      {
          cout << "bind failed. Retrying..." << endl;
      }
      else
      {
         cout << "bind done." << endl;
         isBind = true;
      }
  }
  //Listen to incoming connections
  if(listen(socket_desc , 3)!=0)
  {
    cout << "Cannot listen to the incoming request. Retrying..." << endl;
  }
  else
  {
    cout << "Waiting for incoming connections.." << endl;
          c = sizeof(struct sockaddr_in);
          while(!isAccept)
          {
              cout << "Value of new socket is : " << new_socket << endl;
              new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c);
              if(new_socket < 0)
              {
                cout << "Accept failed." << endl;
                sleep(1);
              }
              else
              {
                isAccept = true;
                cout << "Connection accepted." << endl;
              }
          }
  }

  send_image(new_socket, ReceivedImage);



}
cout << "closing socket" <<endl;
    close(socket_desc);
    fflush(stdout);
    return 0;

}

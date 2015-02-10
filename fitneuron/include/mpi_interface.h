#ifndef _MPI_INTERFACE_H_
#define _MPI_INTERFACE_H_



#include <mpi.h>



//When using MPI code, must add -lmpi -lmpi++ to compile lines
//USE_LINK_FLAG -lmpi -lmpi_cxx
//USE_COMPILER $(MPICCC)
//USE_COMPILE_FLAG -fexceptions


// display some mpi information
inline void displayMpiInfo(std::ostream & out = std::cout);



//Gets the appropriate MPI::Datatype for X
template <class T>
inline MPI::Datatype getMpiType(const T & X);

//Broadcasts
// Synchronize with root, either
//   1) scalar
//   2) array
//   3) container object (std::vector, std::string, etc)
// There are const and non-const versions, because root sends and could
//  therefore be const, but non-root receives, and is therefore non-const
template <class T>
void
BcastMPI(T & val, int root=0, MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
void
BcastMPI(const T & val, int root=0, MPI::Intracomm & comm = MPI::COMM_WORLD);

template <class T>
void BcastArrMPI(T* arr, unsigned long arrLen, int root=0,
                 MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
void BcastArrMPI(T const* arr, unsigned long arrLen, int root=0,
                 MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
inline void
BcastArrMPI(T & arr, int root=0,
            MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
inline void
BcastArrMPI(const T & arr, int root=0,
            MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
inline void
BcastArrMPIWithLength(T & arr, int root=0,
            MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
inline void
BcastArrMPIWithLength(const T & arr, int root=0,
            MPI::Intracomm & comm = MPI::COMM_WORLD);

// Lots of function defs here, but all of the form
//  SendMPI(const T & val, int destination, int & tag = 0,
//          MPI::Intracomm & comm = MPI::COMM_WORLD);
//  SendArrMPI(T const* arr, unsigned long Len, int destination, int & tag = 0,
//             MPI::Intracomm & comm = MPI::COMM_WORLD);
//  SendArrMPI(const T & arr, int destination, int & tag = 0,
//             MPI::Intracomm & comm = MPI::COMM_WORLD);
//  RecvMPI(T & val, int source, int & tag = 0,
//          MPI::Intracomm & comm = MPI::COMM_WORLD);
//  RecvArrMPI(T* arr, unsigned long Len, int source, int & tag = 0,
//             MPI::Intracomm & comm = MPI::COMM_WORLD);
//  RecvArrMPI(T & arr, int source, int & tag = 0,
//             MPI::Intracomm & comm = MPI::COMM_WORLD);

//  SendArrMPI and RecvArrMPI have non-pointer forms that can send container
//  objects (std::vector, std::string, etc).

// Prefix:
//   none = blocking send/receive
//   'I' = non-blocking, regular send/receive
//   'IB' = non-blocking, buffered send

// if tag is passed as an integer, it will be incremented so as to avoid
// collisions with previous/subsequent messages

// return is void for blocking functions, and
//   std::vector<MPI::Request> for non-blocking functions

//Blocking Sends and Receives
template <class T>
void
SendMPI(const T & val, int destination, int & tag=0,
        MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
void
RecvMPI(T & val, int source, int & tag=0,
        MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
void
SendArrMPI(T const* arr, unsigned long arrLen, int destination, int & tag=0,
           MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
void
RecvArrMPI(T* arr, unsigned long arrLen, int source, int & tag=0,
           MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
inline void
SendArrMPI(const T & arr, int destination, int & tag=0,
           MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
inline void
RecvArrMPI(T & arr, int source, int & tag=0,
           MPI::Intracomm & comm = MPI::COMM_WORLD);
                       
//Non-blocking Sends and Receives
template <class T>
MPI::Request
ISendMPI(const T & val, int destination, int & tag=0,
         MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
MPI::Request
IRecvMPI(T & val, int source, int & tag=0,
		     MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
std::vector<MPI::Request>
ISendArrMPI(T const* arr, unsigned long arrLen, int destination, int & tag=0,
  		      MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
std::vector<MPI::Request>
IRecvArrMPI(T* arr, unsigned long arrLen, int source, int & tag=0,
				      MPI::Intracomm & comm = MPI::COMM_WORLD);
				      
template <class T>
inline std::vector<MPI::Request>
ISendArrMPI(const T & arr, int destination, int & tag=0,
            MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
inline std::vector<MPI::Request>
IRecvArrMPI(T & arr, int source, int & tag=0,
            MPI::Intracomm & comm = MPI::COMM_WORLD);

//Buffered non-blocking Sends
template <class T>
MPI::Request
IBSendMPI(const T & val, int destination, int & tag=0,
          MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
std::vector<MPI::Request>
IBSendArrMPI(T const* arr, unsigned long arrLen, int destination, int & tag=0,
				     MPI::Intracomm & comm = MPI::COMM_WORLD);
template <class T>
inline std::vector<MPI::Request>
IBSendArrMPI(const T & arr, int destination, int & tag=0,
            MPI::Intracomm & comm = MPI::COMM_WORLD);



//#############################displayMpiInfo #################################



inline void
displayMpiInfo(std::ostream & out)
{
  // display some mpi information  

  int version, subversion, nameLen;
  char hostName[MPI::MAX_PROCESSOR_NAME];
  MPI::Get_version(version, subversion);
  MPI::Get_processor_name(hostName, nameLen);
    
  out << "using MPIv" << version << '.' << subversion
      << ", master node on " << hostName << '\n';
  out.flush();
}



//############################### getMpiType ##################################



template <>
inline MPI::Datatype getMpiType(const bool & x){ (void) x; return MPI::BOOL; }
template <>
inline MPI::Datatype getMpiType(const char & x){ (void) x; return MPI::CHAR; }
template <>
inline MPI::Datatype getMpiType(const int & x){ (void) x; return MPI::INT; }
template <>
inline MPI::Datatype getMpiType(const long & x){ (void) x; return MPI::LONG; }
template <>
inline MPI::Datatype getMpiType(const unsigned long & x){
  (void) x;  return MPI::UNSIGNED_LONG;
}
template <>
inline MPI::Datatype getMpiType(const double & x)
{
  (void) x; return MPI::DOUBLE;
}
template <class T>
inline MPI::Datatype getMpiType(const T & x){ (void) x; return MPI::BYTE; }



//############################ Broadcasts ################################
template <class T>
void BcastMPI(T & val, int root, MPI::Intracomm & comm)
{  
  int numSend;
  MPI::Datatype type = getMpiType(val);
  if(type == MPI::BYTE)
    numSend = (int)sizeof(T);
  else
    numSend = 1;

  comm.Bcast((void*)&val, numSend, type, root);
}



template <class T>
void BcastMPI(const T & val, int root, MPI::Intracomm & comm)
{  
  int numSend;
  MPI::Datatype type = getMpiType(val);
  if(type == MPI::BYTE)
    numSend = (int)sizeof(T);
  else
    numSend = 1;

  comm.Bcast((void*)&val, numSend, type, root);
}



template <class T>
void BcastArrMPI(T* arr, unsigned long arrLen, int root, MPI::Intracomm & comm)
{
  MPI::Datatype type = getMpiType(*arr);
  if(type == MPI::BYTE) {
    size_t TSize = sizeof(T);
    unsigned long numSend = TSize * arrLen;
    char* charArr = (char*)arr;

    int sendLen;
    while(numSend > 0) {
      if(numSend < 32768)
	      sendLen = (int)numSend;
  	  else
	      sendLen = 32767;
	  
	    comm.Bcast((void*)charArr, sendLen, type, root);
	    numSend -= sendLen;
	    charArr += sendLen;
	  }
  }
  else {
    int sendLen;
    while(arrLen > 0) {
  	  if(arrLen < 32768)
  	    sendLen = (int)arrLen;
  	  else
  	    sendLen = 32767;
	  
  	  comm.Bcast((void*)arr, sendLen, type, root);
  	  arrLen -= sendLen;
  	  arr += sendLen;
  	}
  }
}



template <class T>
void
BcastArrMPI(T const* arr, unsigned long arrLen, int root,
            MPI::Intracomm & comm)
{
  MPI::Datatype type = getMpiType(*arr);
  if(type == MPI::BYTE) {
    size_t TSize = sizeof(T);
    unsigned long numSend = TSize * arrLen;
    char const* charArr = (char const*)arr;

    int sendLen;
    while(numSend > 0) {
      if(numSend < 32768)
	      sendLen = (int)numSend;
  	  else
	      sendLen = 32767;
	  
	    comm.Bcast((void*)charArr, sendLen, type, root);
	    numSend -= sendLen;
	    charArr += sendLen;
	  }
  }
  else {
    int sendLen;
    while(arrLen > 0) {
  	  if(arrLen < 32768)
  	    sendLen = (int)arrLen;
  	  else
  	    sendLen = 32767;
	  
  	  comm.Bcast((void*)arr, sendLen, type, root);
  	  arrLen -= sendLen;
  	  arr += sendLen;
  	}
  }
}



template <class T>
inline void
BcastArrMPI(T & arr, int root, MPI::Intracomm & comm)
{
  BcastArrMPI(&arr[0], arr.size(), root, comm);
}


template <class T>
inline void
BcastArrMPI(const T & arr, int root, MPI::Intracomm & comm)
{
  BcastArrMPI(&arr[0], arr.size(), root, comm);
}



template <class T>
inline void
BcastArrMPIWithLength(T & arr, int root, MPI::Intracomm & comm)
{
  // for root: send size then send array
  // for non-root: receive size, then resize array, then receive array
  size_t len = arr.size();
  BcastMPI(len, root, comm);
  arr.resize(len);
  if(len > 0)
    BcastArrMPI(&arr[0], len, root, comm);
}


template <class T>
inline void
BcastArrMPIWithLength(const T & arr, int root, MPI::Intracomm & comm)
{
  // inherently root only, send size then send array
  BcastMPI(arr.size(), root, comm);
  if(arr.size() > 0)
    BcastArrMPI(&arr[0], arr.size(), root, comm);
}



//################### Blocking Sends and Receives ########################
template <class T>
void SendMPI(const T & val, int destination, int & tag, MPI::Intracomm & comm)
{
  int numSend;
  MPI::Datatype type = getMpiType(val);
  if(type == MPI::BYTE)
    numSend = (int)sizeof(T);
  else
    numSend = 1;

  comm.Send((void*)&val, numSend, type, destination, tag);
  tag++;
}
template <class T>
void RecvMPI(T & val, int source, int & tag, MPI::Intracomm & comm)
{
  int numSend;
  MPI::Datatype type = getMpiType(val);
  if(type == MPI::BYTE)
    numSend = (int)sizeof(T);
  else
    numSend = 1;

  comm.Recv((void*)&val, numSend, type, source, tag);
  tag++;
}


template <class T>
void
SendArrMPI(T const* arr, unsigned long arrLen, int destination, int & tag,
           MPI::Intracomm & comm)
{
  MPI::Datatype type = getMpiType(*arr);
  if(type == MPI::BYTE) {
    size_t TSize = sizeof(T);
    unsigned long numSend = TSize * arrLen;
    char const* charArr = (char const*)arr;

    int sendLen;
    while(numSend > 0) {
  	  if(numSend < 32768)
  	    sendLen = (int)numSend;
  	  else
  	    sendLen = 32767;
	  
  	  comm.Send((void*)charArr, sendLen, type, destination, tag);
  	  tag++;
  	  numSend -= sendLen;
  	  charArr += sendLen;
  	}
  }
  else {
    int sendLen;
    while(arrLen > 0) {
  	  if(arrLen < 32768)
  	    sendLen = (int)arrLen;
  	  else
  	    sendLen = 32767;
	  
  	  comm.Send((void*)arr, sendLen, type, destination, tag);
  	  tag++;
  	  arrLen -= sendLen;
  	  arr += sendLen;
  	}
  }
}



template <class T>
void RecvArrMPI(T* arr, unsigned long arrLen, int source, int & tag,
		            MPI::Intracomm & comm)
{
  MPI::Datatype type = getMpiType(*arr);
  if(type == MPI::BYTE) {
    size_t TSize = sizeof(T);
    unsigned long numSend = TSize * arrLen;
    char* charArr = (char*)arr;

    int sendLen;
    while(numSend > 0) {
  	  if(numSend < 32768)
  	    sendLen = (int)numSend;
  	  else
  	    sendLen = 32767;
	  
  	  comm.Recv((void*)charArr, sendLen, type, source, tag);
  	  tag++;
  	  numSend -= sendLen;
  	  charArr += sendLen;
  	}
  }
  else {
    int sendLen;
    while(arrLen > 0) {
  	  if(arrLen < 32768)
  	    sendLen = (int)arrLen;
	    else
	      sendLen = 32767;
	  
	    comm.Recv((void*)arr, sendLen, type, source, tag);
	    tag++;
	    arrLen -= sendLen;
	    arr += sendLen;
	  }
  }
}



template <class T>
inline void SendArrMPI(const T & arr, int destination, int & tag,
                       MPI::Intracomm & comm)
{
  SendArrMPI(&arr[0], arr.size(), destination, tag, comm);
}



template <class T>
inline void RecvArrMPI(T & arr, int source, int & tag,
                       MPI::Intracomm & comm)
{
  RecvArrMPI(&arr[0], arr.size(), source, tag, comm);
}



//################# Non-blocking Sends and Receives ######################
template <class T>
MPI::Request
ISendMPI(const T & val, int destination, int & tag, MPI::Intracomm & comm)
{
  int numSend;
  MPI::Datatype type = getMpiType(val);
  if(type == MPI::BYTE)
    numSend = (int)sizeof(T);
  else
    numSend = 1;

  MPI::Request request =
    comm.Isend((void*)&val, numSend, type, destination, tag);
  tag++;
  return request;
}



template <class T>
MPI::Request IRecvMPI(T & val, int source, int & tag, MPI::Intracomm & comm)
{
  int numSend;
  MPI::Datatype type = getMpiType(val);
  if(type == MPI::BYTE)
    numSend = (int)sizeof(T);
  else
    numSend = 1;

  MPI::Request request =
    comm.Irecv((void*)&val, numSend, type,	source, tag);
  tag++;
  return request;
}



template <class T>
std::vector<MPI::Request>
ISendArrMPI(T const* arr, unsigned long arrLen, int destination, int & tag,
  		      MPI::Intracomm & comm)
{
  std::vector<MPI::Request> requests;
  MPI::Datatype type = getMpiType(*arr);
  if(type == MPI::BYTE) {
    size_t TSize = sizeof(T);
    unsigned long numSend = TSize * arrLen;
    char const* charArr = (char const*)arr;

    int sendLen;
    while(numSend > 0) {
  	  if(numSend < 32768)
  	    sendLen = (int)numSend;
  	  else
  	    sendLen = 32767;
	  
  	  requests.push_back(comm.Isend((void*)charArr, sendLen, type, destination,
  	                                tag));
  	  tag++;
	    numSend -= sendLen;
	    charArr += sendLen;
	  }
  }
  else {
    int sendLen;
    while(arrLen > 0) {
  	  if(arrLen < 32768)
  	    sendLen = (int)arrLen;
  	  else
  	    sendLen = 32767;
	  
  	  requests.push_back(comm.Isend((void*)arr, sendLen, type, destination,
  	                                tag));
  	  tag++;
	    arrLen -= sendLen;
	    arr += sendLen;
  	}
  }
  return requests;
}


template <class T>
std::vector<MPI::Request>
IRecvArrMPI(T* arr, unsigned long arrLen, int source, int & tag,
            MPI::Intracomm & comm)
{
  std::vector<MPI::Request> requests;
  MPI::Datatype type = getMpiType(*arr);
  if(type == MPI::BYTE) {
    size_t TSize = sizeof(T);
    unsigned long numSend = TSize * arrLen;
    char* charArr = (char*)arr;

    int sendLen;
    while(numSend > 0) {
  	  if(numSend < 32768)
  	    sendLen = (int)numSend;
  	  else
  	    sendLen = 32767;
	  
  	  requests.push_back(comm.Irecv((void*)charArr, sendLen, type, source,
  	                                tag));
  	  tag++;
  	  numSend -= sendLen;
  	  charArr += sendLen;
  	}
  }
  else {
    int sendLen;
    while(arrLen > 0) {
  	  if(arrLen < 32768)
  	    sendLen = (int)arrLen;
  	  else
  	    sendLen = 32767;
	  
  	  requests.push_back(comm.Irecv((void*)arr, sendLen, type, source, tag));
  	  tag++;
	    arrLen -= sendLen;
	    arr += sendLen;
	  }
  }
  return requests;
}



template <class T>
inline std::vector<MPI::Request>
ISendArrMPI(const T & arr, int destination, int & tag, MPI::Intracomm & comm)
{
  return ISendArrMPI(&arr[0], arr.size(), destination, tag, comm);
}



template <class T>
inline std::vector<MPI::Request>
IRecvArrMPI(T & arr, int source, int & tag, MPI::Intracomm & comm)
{
  return IRecvArrMPI(&arr[0], arr.size(), source, tag, comm);
}



template <class T>
inline void
voidISendMPI(const T & val, int destination, int & tag, MPI::Intracomm & comm)
{
  int numSend;
  MPI::Datatype type = getMpiType(val);
  if(type == MPI::BYTE)
    numSend = (int)sizeof(T);
  else
    numSend = 1;

  // send value and free request
  comm.Isend((void*)&val, numSend, type, destination, tag).Free();
  tag++;
}


template <class T>
inline void
voidIBSendMPI(const T & val, int destination, int & tag, MPI::Intracomm & comm)
{
  int numSend;
  MPI::Datatype type = getMpiType(val);
  if(type == MPI::BYTE)
    numSend = (int)sizeof(T);
  else
    numSend = 1;

  // send value and free request
  comm.Ibsend((void*)&val, numSend, type, destination, tag).Free();
  tag++;
}


template <class T>
void
voidISendArrMPI(T const* arr, unsigned long arrLen, int destination, int & tag,
                MPI::Intracomm & comm)
{
  MPI::Datatype type = getMpiType(*arr);
  static MPI::Request tempRequest;
  
  if(type == MPI::BYTE) {
    size_t TSize = sizeof(T);
    unsigned long numSend = TSize * arrLen;
    char const* charArr = (char const*)arr;

    int sendLen;
    while(numSend > 0) {
  	  if(numSend < 32768)
  	    sendLen = (int)numSend;
  	  else
  	    sendLen = 32767;
	  
  	  tempRequest = comm.Isend((void*)charArr, sendLen, type, destination,
                               tag);
      tempRequest.Free();
  	  tag++;
	    numSend -= sendLen;
	    charArr += sendLen;
	  }
  }
  else {
    int sendLen;
    while(arrLen > 0) {
  	  if(arrLen < 32768)
  	    sendLen = (int)arrLen;
  	  else
  	    sendLen = 32767;
	  
  	  tempRequest = comm.Isend((void*)arr, sendLen, type, destination,
  	                                tag);
      tempRequest.Free();
  	  tag++;
	    arrLen -= sendLen;
	    arr += sendLen;
  	}
  }
}



template <class T>
inline void
voidISendArrMPI(const T & arr, int destination, int & tag, MPI::Intracomm & comm)
{
  voidISendArrMPI(&arr[0], arr.size(), destination, tag, comm);
}


template <class T>
void
voidIBSendArrMPI(T const* arr, unsigned long arrLen, int destination, int & tag,
                MPI::Intracomm & comm)
{
  MPI::Datatype type = getMpiType(*arr);
  static MPI::Request tempRequest;
  
  if(type == MPI::BYTE) {
    size_t TSize = sizeof(T);
    unsigned long numSend = TSize * arrLen;
    char const* charArr = (char const*)arr;

    int sendLen;
    while(numSend > 0) {
  	  if(numSend < 32768)
  	    sendLen = (int)numSend;
  	  else
  	    sendLen = 32767;
	  
  	  tempRequest = comm.Ibsend((void*)charArr, sendLen, type, destination,
                               tag);
      tempRequest.Free();
  	  tag++;
	    numSend -= sendLen;
	    charArr += sendLen;
	  }
  }
  else {
    int sendLen;
    while(arrLen > 0) {
  	  if(arrLen < 32768)
  	    sendLen = (int)arrLen;
  	  else
  	    sendLen = 32767;
	  
  	  tempRequest = comm.Ibsend((void*)arr, sendLen, type, destination,
  	                                tag);
      tempRequest.Free();
  	  tag++;
	    arrLen -= sendLen;
	    arr += sendLen;
  	}
  }
}



template <class T>
inline void
voidIBSendArrMPI(const T & arr, int destination, int & tag, MPI::Intracomm & comm)
{
  voidIBSendArrMPI(&arr[0], arr.size(), destination, tag, comm);
}


//############# Buffered Non-blocking Sends and Receives #################
template <class T>
MPI::Request
IBSendMPI(const T & val, int destination, int & tag, MPI::Intracomm & comm)
{
  int numSend;
  MPI::Datatype type = getMpiType(val);
  if(type == MPI::BYTE)
    numSend = (int)sizeof(T);
  else
    numSend = 1;

  MPI::Request request =
    comm.Ibsend((void*)&val, numSend, type, destination, tag);
  tag++;
  return request;
}



template <class T>
std::vector<MPI::Request>
IBSendArrMPI(T const* arr, unsigned long arrLen, int destination, int & tag,
				     MPI::Intracomm & comm)
{
  std::vector<MPI::Request> requests;
  MPI::Datatype type = getMpiType(*arr);
  if(type == MPI::BYTE) {
    size_t TSize = sizeof(T);
    unsigned long numSend = TSize * arrLen;
    char const* charArr = (char const*)arr;

    int sendLen;
    while(numSend > 0) {
  	  if(numSend < 32768)
  	    sendLen = (int)numSend;
  	  else
  	    sendLen = 32767;
	  
  	  requests.push_back(comm.Ibsend((void*)charArr, sendLen, type,
  	                                 destination, tag));
  	  tag++;
	    numSend -= sendLen;
	    charArr += sendLen;
	  }
  }
  else {
    int sendLen;
    while(arrLen > 0) {
  	  if(arrLen < 32768)
  	    sendLen = (int)arrLen;
  	  else
  	    sendLen = 32767;
	  
  	  requests.push_back(comm.Ibsend((void*)arr, sendLen, type, destination,
  	                                tag));
  	  tag++;
	    arrLen -= sendLen;
	    arr += sendLen;
	  }
  }
  return requests;
}



template <class T>
inline std::vector<MPI::Request>
IBSendArrMPI(const T & arr, int destination, int & tag, MPI::Intracomm & comm)
{
  return IBSendArrMPI(&arr[0], arr.size(), destination, tag, comm);
}

#endif

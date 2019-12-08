struct Msg;

typedef void (*Actor) (struct Msg msg);

struct Msg{
  Actor sender;
  Actor recipient;
  int msg_type;
  char *data;
};

// The prefix "AM" is for ActorModel
#define MAX_MSGS 48
Msg AM_msgs[MAX_MSGS];
int AM_head = 0;
int AM_tail = 0;
int AM_count = 0;

void AM_post(struct Msg msg) {
  if(AM_count > MAX_MSGS-1) {
    Serial.println("queue full");
    return;
  }

  int index;
  if(AM_tail == MAX_MSGS) {
    index = 0;
  } else {
    index = AM_tail;
  }
  AM_tail = index + 1;
  
  AM_msgs[index] = msg;
  
  AM_count++;
}

void AM_reply(struct Msg msg, int msg_type, char *data) {
  struct Msg new_msg;
  new_msg.sender = msg.recipient;
  new_msg.recipient = msg.sender;
  new_msg.msg_type = msg_type;
  new_msg.data = data;
  
  AM_post(new_msg);
}

void AM_deliver() {
  if(!AM_count) {
    Serial.println("no msgs");
    return;
  }
  
  struct Msg &msg = AM_msgs[AM_head];
  Actor actor = msg.recipient;
  actor(msg);

  AM_head++;
  if(AM_head == MAX_MSGS) {
    AM_head = 0;
  }
  
  free(msg.data);

  AM_count--;
};

#define ACTOR(NAME, DATA_STRUCT, FUNCTION_BODY) \
void NAME(Msg msg) { \
  static struct DATA_STRUCT self; \
  FUNCTION_BODY \
};


ACTOR(
  player1,
  
  {
    String name = "player1";
  },
  
  Serial.println(self.name + " got '" + msg.data + "'");
  
  char *data = (char*)(malloc(64));
  data[0] = 0;

  if(strcmp(msg.data, "ping") == 0 ) {
    strcpy(data, "pong");
  } else {
    strcpy(data, "ping");
  }
  
  AM_reply(msg, 0, data);
)

ACTOR(
  player2,
  
  {
    String name = "player2";
  },
  
  Serial.println(self.name + " got '" + msg.data + "'");
  
  char *data = (char*)(malloc(64));
  data[0] = 0;

  if(strcmp(msg.data, "ping") == 0 ) {
    strcpy(data, "pong");
  } else {
    strcpy(data, "ping");
  }
  
  AM_reply(msg, 0, data);
)


void setup()
{
  Serial.begin(152000);
  struct Msg msg;
  msg.sender = player1;
  msg.recipient = player2;
  
  char *data = (char*)(malloc(64));
  data[0] = 0;
  strcpy(data, "ping");
  
  msg.data = data;
  
  AM_post(msg);
}

void loop()
{
  AM_deliver();
}



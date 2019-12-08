class Actor;

struct Msg{
  Actor *sender;
  Actor *recipient;
  int msg_type;
  char *data;
};

class Actor {
public:
  virtual void handle_msg(struct Msg &msg) = 0;  // To be implemented by each subclass
};

#define MAX_MSGS 50
class Messenger {
private:
  struct Msg msgs[MAX_MSGS];
  int head = 0;
  int tail = 0;
  int count = 0;
public:
  Messenger() {};
  
  void post(struct Msg msg) {
    //Serial.println("push");
    if(this->count > MAX_MSGS-1) {
      Serial.println("queue full");
      return;
    }

    int index;
    if(this->tail == MAX_MSGS) {
      index = 0;
    } else {
      index = this->tail;
    }
    this->tail = index + 1;
  
    this->msgs[index] = msg;
    
    this->count++;
   
  };
  
  void reply(struct Msg msg, int msg_type, char *data) {
    struct Msg new_msg;
    new_msg.sender = msg.recipient;
    new_msg.recipient = msg.sender;
    new_msg.msg_type = msg_type;
    new_msg.data = data;
    
    this->post(new_msg);
  };

  void deliver() {
    //Serial.println("deliver");
    if(!this->count) {
      Serial.println("no msgs");
      return;
    }
  
    struct Msg &msg = this->msgs[this->head];
    Actor *actor = msg.recipient;
    actor->handle_msg(msg);

    this->head++;
    if(this->head == MAX_MSGS) {
      this->head = 0;
    }
    
    free(msg.data);
  
    this->count--;
  };
};

Messenger messenger = Messenger();

class Player : public Actor {
private:
  String _name;
public:
  Player(String name) {
    this->_name = name;
  };
  void handle_msg(struct Msg &msg) {
    Serial.println(this->_name + " got'" + msg.data + "'");

    char *data = (char*)(malloc(64));
    
    if(strcmp(msg.data,"ping") == 0) {
      strcpy(data, "pong");
    } else {
      strcpy(data, "ping");
    }
  
    messenger.reply(msg, 0, data);
  };
};

Player player1("player1");
Player player2("player2");

unsigned int count = 0;

void setup() {
  Serial.begin(115200);
  struct Msg msg;
  msg.sender = (Actor *)&player1;
  msg.recipient = (Actor *)&player2;
  
  msg.data = (char*)(malloc(64));
  msg.data[0] = 0;
  strcpy(msg.data, "ping");
  
  messenger.post(msg);
}

void loop() {
  messenger.deliver();
}

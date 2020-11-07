#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;


//i wonder if we can modify this function
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

int free_lth(){
  WP* p=free_;
  int lth=0;
  while(p!=NULL){
    lth++;
    p=p->next;
  }
  return lth;
}

WP* new_wp(){
  if(free_lth()<=0){
    assert(0);
  }
  WP* p=free_; //store now alloc node
  free_=free_->next;
  p->next=head; //insert new wp at the head of liner table 
  head=p;

  return head;
}

void free_wp(WP* wp){
  if(wp==NULL)
    return;
  WP *temp=head, *last=head;
  if(wp->NO==head->NO){ //free head node, link it to free head
    head=wp->next;
    wp->next=free_;
    free_=wp;
  }
  else{
    while(temp!=NULL&&temp->NO!=wp->NO){
      last=temp;
      temp=temp->next;
    }
    last->next=temp->next;
    temp->next=free_;
    free_=temp;
  }
}

void display_watchpoint(){
  if(head==NULL){
    printf("No watchpoints\n");
    return;
  }

  WP* p=head;
  while(p!=NULL){
    printf("%d\t%s\t%x\n",p->NO,p->exp,p->value);
    p=p->next;
  }

}

bool delete_wp(int num){
  if(head==NULL){
    printf("No watchpoints\n");
    return false;
  }
  WP *p=head;
  while(p!=NULL){
    if(p->NO==num){
      free_wp(p);
      return true;
    }
  }
  return false;

}

WP* get_head_pointer(){
  WP* p=head;
  return p;
}

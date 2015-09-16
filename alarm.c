#include <stdio.h>
#include <list.h>
#include <alarm.h>
#include <stdlib.h>

struct fit figure[5] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
struct alarm tick_queue[7];

int tick_queue_init(void)
{
	U8 i;
	for (i = 0; i < 7; i++)
        	list_init(&tick_queue[i].list);

	return 0;

}

int tick_queue_insert(struct alarm *tick)
{
	list_insert_behind(&tick_queue[tick->week].list, &tick->list);

	return 0;
}

int tick_queue_delete(struct alarm *tick)
{
	list_delete(&tick->list);

	return 0;
}

/*traverse the alarm list to search alarm id*/
struct alarm *search_alarm(char id)
{
	int i;
	struct alarm *tick_tmp;
	for (i = 0; i < 7; i++) {
		LIST *tmp = &tick_queue[i].list;
		while (!is_list_last(tmp)) {
			tick_tmp = list_entry(tmp->next, struct alarm, list);
			tmp = tmp->next;
#if 0
			printf("id: %d, week: %d, hour: %d, minute: %d, second: %d, repeat: %d\n",
					tick_tmp->id, tick_tmp->wflag, tick_tmp->hour, tick_tmp->minute,
					tick_tmp->second, tick_tmp->rflag);
#endif
			if (id == tick_tmp->id)
				return tick_tmp;
		}
	}

	return NULL;
}

void week_init()
{
	int i;
	for (i = 0; i < 7; i++) {
		_week[week_bitmap[i]] = i;
	}
}

int main()
{
	tick_queue_init();
	week_init();
	addtimer(0, 5, 3, 1, 1, 1);
	addtimer(1, 0, 2, 1, 0, 1);
	addtimer(2, 2, 4, 1, 10, 1);
	addtimer(3, 3, 1, 1, 20, 1);
	addtimer(4, 4, 6, 1, 23, 1);

	struct alarm *tick_tmp = get_new_alarm();
	if (tick_tmp == NULL) {
		printf("no alarm get\n");
		return 0;
	}

	printf("id: %d, week: %d, hour: %d, minute: %d, second: %d\n",
		tick_tmp->id, _week[tick_tmp->wflag], tick_tmp->hour, tick_tmp->minute,
		tick_tmp->second);

	return 0;
}

U32 addtimer(U8 id,
		U8 week,
		U8 hour,
		U8 minute,
		U8 second,
		bool repeat)
{
	if (!((id >=0) && (week >= 0) && (week < 7) &&
		(hour >= 0) && (hour < 25) && (minute >= 0) &&
		(minute < 61) && (second >= 0) && (second < 61)))
	{
		for (;;)
			printf("error argument \n");
	}

	/*traverse the alarm list to search alarm id*/
	int i;
	for (i = 0; i < 7; i++) {
		struct alarm *tick_tmp;
		LIST *tmp = &tick_queue[i].list;

		while (!is_list_last(tmp)) {
			tick_tmp = list_entry(tmp->next, struct alarm, list);
			tmp = tmp->next;

			if (tick_tmp->id == id) {
				printf("id: %d, week: %d, hour: %d, minute: %d, second: %d, repeat: %d\n",
						tick_tmp->id, tick_tmp->wflag, tick_tmp->hour, tick_tmp->minute,
						tick_tmp->second, tick_tmp->rflag);
				printf("============>alarm exist<==============\n");
				return EXIST;
			}
		}
	}

	/*if alarm id not exist in the queue, then add it*/
	struct alarm *node = (struct alarm *)malloc(sizeof(struct alarm));
	node->id = id;
	/*bitmap for which day*/
	node->wflag |= (1 << week);
	node->week = week;
	node->minute = minute;
	node->second = second;
	node->hour = hour;

	node->run |= (1<<week);
	if (repeat) {
		node->rflag |= (1 << week);
	}

	tick_queue_insert(node);
	return SUCCESS;
}

struct alarm *get_new_alarm(void)
{
	struct alarm *now = system_timer_get();
	int i = 0;
	
	struct alarm *tick_tmp;
	for (i = now->week; i < 7; i++) {
		int j = 0;
		LIST *tmp = &tick_queue[i].list;
		while (!is_list_last(tmp)) {
			tick_tmp = list_entry(tmp->next, struct alarm, list);
			tmp = tmp->next;

			if (tick_tmp->run & (1 << i)) {
				figure[j].id = tick_tmp->id;
				figure[j].num = tick_tmp->hour;
				j++;
			}
		}
		int id = compare(now->hour, figure);
		if (id != -1) {
			printf("===============>id: %d <================\n", id);
			return search_alarm(id);
		}

	}
	/*no alarm*/
	return NULL;
}

int compare(char c, struct fit n[5])
{
	int i;
	int j = 0;
	char copy[5] = {100, 100, 100, 100, 100};
	for (i = 0; i < 5; i++) {
		if ((n[i].num - c) >= 0) {
			copy[i] = n[i].num - c;
			j++;
		}
	}
	if (!j)
		return -1;

	i = min(copy);

	return n[i].id;
}

int min(char a[5])
{
	char m = a[0];
	char x;
	int i;
	for (i = 0; i < 5; i++) {
		if (a[i] < m) {
			m = a[i];
			x = i;
		}
	}

	printf("最小的数是%d,他在a[%d]位置\n", m, x);
	return x;
}

struct alarm *system_timer_get(void)
{
	struct alarm *tmp = malloc(sizeof(struct alarm));
	tmp->week = 1;
	tmp->hour = 1;
	tmp->minute = 1;
	tmp->second = 1;

	return tmp;
}

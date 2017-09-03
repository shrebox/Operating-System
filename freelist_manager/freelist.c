#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

struct header
{
	int size;
	int is_free;
	struct header *next;
	
};

struct header *head = NULL, *tail = NULL;
pthread_mutex_t global_malloc_lock;
int countt,bsize,pos;
char* line;

struct header *get_free_block(int size)
{
	struct header *curr = head;

	while(curr)
	{
		if(curr->is_free && curr->size >=size)
		{
			return curr;
		}

		curr = curr->next;
	}

	return NULL;
}

void ffree(void *block)
{
	struct header *h,*tmp;
	void*programbreak;

	if(!block)
	{
		return;
	}

	else
	{
		pthread_mutex_lock(&global_malloc_lock);
		h = (struct header *)block - 1;
		programbreak = sbrk(0);

		if((char*)block + h->size == programbreak)
		{
			if(head==tail)
			{
				head = tail = NULL;

			}

			else
			{
				tmp = head;

				while(tmp)
				{
					if(tmp->next == tail)
					{
						tmp->next = NULL;
						tail = tmp;
					}

					tmp = tmp->next;
				}
			}

			sbrk(0- h->size -sizeof(struct header));

			pthread_mutex_unlock(&global_malloc_lock);
			countt--;
			return;
		}

		else
		{
			h->is_free = 1;
			pthread_mutex_unlock(&global_malloc_lock);
		}
	}
}

void *mmalloc(int size)
{
	int total_size;
	void*block;
	struct header *h;

	if(size==0)
	{
		return NULL;
	}

	else
	{

		pthread_mutex_lock(&global_malloc_lock);

		h = get_free_block(size);

		if(h)
		{
			h->is_free = 0;
			pthread_mutex_unlock(&global_malloc_lock);
			return (void*)(h+1);
		}

		else
		{

			total_size = sizeof(struct header) + size;
			block = sbrk(total_size);

			if(block==(void*)-1)
			{
				pthread_mutex_unlock(&global_malloc_lock);
				return NULL;
			}

			else
			{

				h = (struct header*)block;
				h->size = size;
				h->is_free = 0;
				h->next = NULL;

				if(!head)
				{
					head = h;
				}

				if(tail)
				{
					tail->next = h;
				}

				tail = h;
				countt++;

				pthread_mutex_unlock(&global_malloc_lock);
				return (void*)(h+1);
			}
		}
	}

}

void allocerror()
{
	
	fprintf(stderr, "Allocation Error\n");
	exit(EXIT_FAILURE);

}

int main()
{
	int sizee,place;
	void*arr[100005];
	countt = 1;

	while(1)
	{
		printf("> ");

		int c;
		pos = 0;
		bsize = 1024;

		line  = malloc(sizeof(char)*1024);
		if(!line)
			allocerror();

		while(1)
		{
			c = getchar();

			if(c==' ' || c=='\n')
			{
				line[pos] = '\0';
				break;
			}

			else
			{
				line[pos] = c;
			}

			pos++;


			if(pos>=bsize)
			{
				bsize += 1024;
				line = realloc(line,bsize);
				if(!line)
					allocerror();
			}
		}

		if(strcmp(line,"")!=0)
		{
			if(strcmp(line,"malloc")==0)
			{
				scanf("%d", &sizee);

				arr[countt] = mmalloc(sizee);
			}

			else if(strcmp(line,"free")==0)
			{
				scanf("%d", &place);

				if(countt>1)
				{
					ffree(arr[place]);
				}
			}

			else if(strcmp(line,"print")==0)
			{
				int cc = 1;
				struct header *curr = head;
				printf("head = %p, tail = %p \n", (void*)head, (void*)tail);

				while(curr)
				{
					printf(" %d  addr = %p, size = %d, is_free=%d, next=%p\n", cc, (void*)curr, curr->size, curr->is_free, (void*)curr->next);
					curr = curr->next;
					cc++;
				}

			}

			else if(strcmp(line,"exit")==0)
			{
				break;
			}
		}

	}
}

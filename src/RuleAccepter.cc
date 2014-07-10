#include "RuleAccepter.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <boost/thread/thread.hpp>

RuleAccepter::RuleAccepter(int p):response("ok")
{
	sprintf(port, "%d\0", p);
	sfd = create_and_bind(port);
}

RuleAccepter& RuleAccepter::instance()
{
	static RuleAccepter ruleAccepter;
	return ruleAccepter;
}

int RuleAccepter::make_socket_non_blocking (int sfd)
{
	int flags, s;

	flags = fcntl (sfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror ("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl (sfd, F_SETFL, flags);
	if (s == -1)
	{
		perror ("fcntl");
		return -1;
	}

	return 0;
}

int RuleAccepter::create_and_bind (char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sfd;

	memset (&hints, 0, sizeof (struct addrinfo));
	hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE;     /* All interfaces */

	s = getaddrinfo (NULL, port, &hints, &result);
	if (s != 0)
	{
		fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0)
		{
			/* We managed to bind successfully! */
			break;
		}

		close (sfd);
	}

	if (rp == NULL)
	{
		fprintf (stderr, "Could not bind\n");
		return -1;
	}

	freeaddrinfo (result);

	if(-1 == make_socket_non_blocking(sfd))
		abort ();

	return sfd;
}

void RuleAccepter::startSever()
{
	int s, efd;
	struct epoll_event event;
	struct epoll_event *events;
  
	s = listen (sfd, SOMAXCONN);
	printf("start listen\n");
	if (s == -1)
	{
		perror ("listen");
		abort ();
	}

	efd = epoll_create1 (0);
	if (efd == -1)
	{
		perror ("epoll_create");
		abort ();
	}

	event.data.fd = sfd;
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
	if (s == -1)
	{
		perror ("epoll_ctl");
		abort ();
	}

	/* Buffer where events are returned */
	events = (struct epoll_event *)calloc(MAXEVENTS, sizeof event);

	/* The event loop */
	while (1)
	{
		int n, i;

		n = epoll_wait (efd, events, MAXEVENTS, -1);
		for (i = 0; i < n; i++)
		{
			if ((events[i].events & EPOLLERR) ||
				(events[i].events & EPOLLHUP) ||
				(!(events[i].events & EPOLLIN)))
			{
				/* An error has occured on this fd, or the socket is not
				 *				ready for reading (why were we notified then?) */
				fprintf (stderr, "epoll error\n");
				close (events[i].data.fd);
				continue;
			}

			if (sfd == events[i].data.fd)
			{
				/* We have a notification on the listening socket, which
				 *				means one or more incoming connections. */
				while (1)
				{
					struct sockaddr in_addr;
					socklen_t in_len;
					int infd;
					char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

					in_len = sizeof in_addr;
					infd = accept (sfd, &in_addr, &in_len);
					if (infd == -1)
					{
						if ((errno == EAGAIN) ||
							(errno == EWOULDBLOCK))
						{
							/* We have processed all incoming
							 *							connections. */
							break;
						}
						else
						{
							perror ("accept");
							break;
						}
					}

					s = getnameinfo (&in_addr, in_len,
									 hbuf, sizeof hbuf,
									 sbuf, sizeof sbuf,
									 NI_NUMERICHOST | NI_NUMERICSERV);
					if (s == 0)
					{
						printf("Accepted connection on descriptor %d "
						"(host=%s, port=%s)\n", infd, hbuf, sbuf);
					}

					/* Make the incoming socket non-blocking and add it to the
					 *					list of fds to monitor. */
					s = make_socket_non_blocking (infd);
					if (s == -1)
						abort ();

					event.data.fd = infd;
					event.events = EPOLLIN | EPOLLET;
					s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);
					if (s == -1)
					{
						perror ("epoll_ctl");
						abort ();
					}
				}
				continue;
			}
		    if(events[i].events & EPOLLIN)
			{
				char buf[MAXBYTES];
				ssize_t count = read (events[i].data.fd, buf, sizeof buf);
				//printf("errno:%d",errno);
               printf("%s\n",buf);
			   
			  
			   
			   
			   if(buf[0]=='R')
				ruleHandler.addRule(buf, count);
               else if(buf[0]=='K')
                kwHandler.addKeyword(buf,count);
			   else if(buf[0]=='P'&&buf[1]=='R')
				   pzid.addPz_id(buf,count);
			   else if(buf[0]=='P'&&buf[1]=='K')
				   pzkw.addPz_kw(buf,count);
		
				   
			   
			   
				   //printf("after errno:%d\n",errno);
			   //printf("it is ok now\n");
			   
			   
				events[i].events = EPOLLOUT | EPOLLET;
				s = epoll_ctl (efd, EPOLL_CTL_MOD, events[i].data.fd, &events[i]);
				if (s == -1)
				{
					perror ("epoll_ctl");
					abort ();
				}
			}
		    if(events[i].events & EPOLLOUT)
			{
				send(events[i].data.fd, response.c_str(), response.length(), 0);
				events[i].events = EPOLLIN | EPOLLET;
				s = epoll_ctl (efd, EPOLL_CTL_MOD, events[i].data.fd, &events[i]);
				if (s == -1)
				{
					perror ("epoll_ctl");
					abort ();
				}
			}
		}
	}
	free (events);
	close (sfd);
}

void RuleAccepter::startAccept()
{
	boost::thread thrd(boost::bind(&RuleAccepter::startSever, &RuleAccepter::instance()));
	thrd.detach();
}

//R < Subject Action ControlTimeFrom ControlTimeTo ContentId
//K < Keyword StreamType ControlTimeFrom controlTimeTo//time fromat:2014-Jul-08 23:04:31


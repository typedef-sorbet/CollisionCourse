CollisionCourse.exe: CollisionCourse.o
	gcc -o CollisionCourse CollisionCourse.o -lncurses
Collisioncourse.o: CollisionCourse.c
	gcc -c CollisionCourse.c

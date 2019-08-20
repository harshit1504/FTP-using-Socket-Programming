CREATE DATABASE Assgn2;
USE Assgn2;
CREATE TABLE dept( 
	d_id INT, 
	d_name varchar(50) NOT NULL, 
	primary key(d_id)
);

CREATE TABLE students( 
	s_id INT, 
	s_name varchar(50) not null, 
	d_id INT, 
	primary key(s_id), 
	foreign key(d_id) references dept(d_id)
);

CREATE TABLE lecturer( 
	l_id INT UNIQUE, 
	l_name varchar(50) not null, 
	d_id INT, 
	primary key(l_id, d_id), 
	foreign key(d_id) references dept(d_id)
);

CREATE TABLE courses( 
	c_id INT UNIQUE , 
	c_name varchar(50) not null, 
	d_id INT, 
	primary key(c_id, d_id), 
	foreign key(d_id) references dept(d_id)
);

CREATE TABLE lc_rel(
	l_id INT,
	c_id INT,
	d_id INT,
	foreign key(l_id, d_id) references lecturer(l_id, d_id),
	foreign key(c_id, d_id) references courses(c_id, d_id)
);

CREATE TABLE sc_rel( 
	c_id INT, 
	s_id INT,
	d_id INT,
	foreign key(c_id, d_id) references courses(c_id, d_id), 
	foreign key(s_id) references students(s_id)
);

INSERT 




select * from students,sc_rel,lc_rel where lc_rel.c_id=sc_rel.c_id and lc_rel.l_id=1 and lc_rel.d_id=sc_rel.d_id and sc_rel.s_id=students.id;
select * from students,courses,sc_rel where courses.c_id=sc_rel.c_id and sc_rel.c_id=1 and students.s_id=sc_rel.s_id;

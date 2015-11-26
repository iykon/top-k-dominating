# top-k-dominating(tkd)
extension of postgresql, top-k-dominating query of incomplete database

## Algorithm Discription
* Please see Mr. Gao's paper: [Top-k Dominating Queries on Incomplete Data] (https://github.com/iykon/top-k-dominating/blob/master/TKD-algorithm.pdf)

## Testing Environment
  * Operation System: Mac OSX / Ubuntu 14.04
  * PostgreSQL version: PostgreSQL 9.3

## How to use?
### 0. Install postgre environment fist

for ubuntu:

~~~terminal
  sudo apt-get install postgresql-9.3
  sudo apt-get install postgresql-contrib-9.3
  sudo apt-get install postgresql-server-dev-9.3
~~~

for mac:

~~~terminal
  brew install postgresql
~~~

### 1. Clone and enter this repository
~~~terminal
    git clone git@github.com:iykon/top-k-dominating.git
    cd top-k-dominating
~~~

In terminal you will see the "tkd.sql" followed by a contrib path like this:
"/usr/share/postgresql/9.3/contrib"

### 3. Import tkd function (in postgresql)

1. Login in postgres with the database that you want to add function to

  (if you want to add function to **all** database, then you should login in **template1** database,
google it for more details.)

2. Use postgresql command to import function like below

~~~sql
    \i /usr/share/postgresql/9.3/contrib/tkd.sql
~~~

3. You will see two "CREATE FUNCTION" outputed if everything ok

### 4. Performing tkd query with warehouse algorithem
Here is a database sample given in the paper:
~~~sql
postgres=> select * from tkd;

 name | d1 | d2 | d3 | d4
------+----+----+----+----
 B3   |    |    |  4 |  9
 D2   |  2 |  1 |    |  4
 C2   |  2 |    |    |  1
 B2   |    |    |  3 |  1
 C4   |  3 |    |    |  3
 A2   |    |  1 |  2 |  1
 D5   |  5 |  5 |    |  4
 D1   |  3 |  5 |    |  2
 D3   |  2 |  4 |    |  1
 B1   |    |    |  1 |  2
 C1   |  2 |    |    |  3
 A4   |    |  7 |  4 |  5
 C3   |  3 |    |    |  2
 B5   |    |    |  7 |  4
 C5   |  3 |    |    |  4
 A3   |    |  1 |  3 |  4
 A5   |    |  4 |  8 |  3
 B4   |    |    |  3 |  7
 A1   |    |  3 |  1 |  3
 D4   |  4 |  4 |    |  5
(20 rows)
~~~

Now since we have created tkd function in the server, we can run tkd query as following format:
~~~sql
postgres=> select * from tkd('select name,d1,d2,d3,d4 from tkd',3,0) as (name text,d1 int, d2 int, d3 int, d4 int);
 name | d1 | d2 | d3 | d4
------+----+----+----+----
 C2   |  2 |    |    |  1
 A2   |    |  1 |  2 |  1
 B2   |    |    |  3 |  1
(3 rows)

~~~

Note that tkd function needs three parameters:

1. the selection clause, can not be NULL (text type)
2. k (integer type)
3. dominating method(integer type). Zero indicates a dominates b if a is smaller than b, non-zero number indicates a dominates b if a is larger than b. Notice that this parameter is optional, leaving unspecified indicating zero 

(ps: you should also provide the output format using as clause, otherwise it won't work).

## Contact us
1. If you have any question about this paper, you can contact Mr. Gao: gaoyj@zju.edu.cn
2. The projet is coded by Weida Pan (Iykon), feel free to ask any questions: iykoncoc001@gmail.com

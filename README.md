# top-k-dominating(tkd)
An extension of postgresql, top-k-dominating query of incomplete database.

## Algorithm Discription
* Please refer to Dr. Gao's paper: [Top-k Dominating Queries on Incomplete Data](https://github.com/iykon/top-k-dominating/blob/master/TKD-algorithm.pdf)

## Testing Environment
  * Operating system: Mac OSX / Ubuntu 14.04
  * PostgreSQL version: PostgreSQL 9.3

## How to use?
### 0. Install postgre environment

For Ubuntu:

~~~terminal
  sudo apt-get install postgresql-9.3
  sudo apt-get install postgresql-contrib-9.3
  sudo apt-get install postgresql-server-dev-9.3
~~~

For Mac:

~~~terminal
  brew install postgresql
~~~

### 1. Clone and enter this repository
~~~terminal
  git clone git@github.com:iykon/top-k-dominating.git
  cd top-k-dominating
~~~

In terminal, you will find the "tkd.sql" followed by a contrib path. Example:
~~~
"/usr/share/postgresql/9.3/contrib"
~~~

### 3. Import tkd function (in postgresql)

(1) Log into postgres with the database that you want to add function to.

(If you want to add function to **all** database, log into **template1** database. More details can be found [here](https://www.postgresql.org/docs/9.3/static/manage-ag-templatedbs.html))

(2) Use postgresql command to import function. Example:

~~~sql
    \i /usr/share/postgresql/9.3/contrib/tkd.sql
~~~

(3) If everything is fine, two "CREATE FUNCTION" should be outputed.

### 4. Perform tkd query with warehouse algorithm
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

Now since we have created tkd function in the server, we can run tkd query in the following format:
~~~sql
postgres=> select * from tkd('select name,d1,d2,d3,d4 from tkd',3,0) as (name text,d1 int, d2 int, d3 int, d4 int);
 name | d1 | d2 | d3 | d4
------+----+----+----+----
 C2   |  2 |    |    |  1
 A2   |    |  1 |  2 |  1
 B2   |    |    |  3 |  1
(3 rows)

~~~

Note that tkd function needs three parameters:<br>
1. The selection clause (text type)
* This clause can not be NULL.
2. K (integer type)
* Rows to output.
3. Dominating method (integer type)
* 0 indicates a dominates b if a is smaller than b.
* Non-zero number indicates a dominates b if a is larger than b. 
* This parameter is optional and the default value is zero.

(ps: you should also provide the output format using as clause, otherwise it won't work).

## Contact us
1. If you have any question about this paper, you can contact Dr. Gao: gaoyj@zju.edu.cn
2. The projet is coded by Weida Pan (Iykon), feel free to ask any questions: iykoncoc001@gmail.com

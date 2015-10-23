class test_result;
  virtual function void display();
  endfunction
endclass

class sv_skip extends test_result;
  integer fd;

  function new();

  endfunction

  virtual function void display();
    $fdisplay( fd, "\t\t\t\t<skipped/>");
  endfunction

endclass

class sv_failure extends test_result;
  integer fd;

  string failure_type;
  string failure_message;
  string log;

  function new(string failure_type, string failure_message, string log);
    this.failure_type = failure_type;
    this.failure_message = failure_message;
    this.log = log;
  endfunction


  virtual function void display();
    $fdisplay( fd, "\t\t\t\t<failure type=\"%s\" message=\"%s\">", failure_type, failure_message);
    $fdisplay( fd, "%s", log );
    $fdisplay( fd, "\t\t\t\t</failure>");
  endfunction
endclass

class sv_error extends test_result;
  integer fd;

  string error_type;
  string error_message;
  string log;

  function new(string error_type, string error_message, string log);
    this.error_type = error_type;
    this.error_message = error_message;
    this.log = log;
  endfunction


  virtual function void display();
    $fdisplay( fd, "\t\t\t\t<error type=\"%s\" message=\"%s\">", error_type, error_message);
    $fdisplay( fd, "%s", log );
    $fdisplay( fd, "\t\t\t\t</error>");
  endfunction

endclass

class sv_testcase;
  test_result result;

  integer fd;
  integer sum_of_error;
  integer sum_of_fatal;

  string classname;
  string name;

  function new();
    sum_of_error = 0;
    sum_of_fatal = 0;
  endfunction

  virtual function void setName(string classname, string name);
    this.classname = classname;
    this.name = name;
  endfunction

  virtual function void add_failure(string failure_type, string failure_message, string log);
    sv_failure t_failure = new(failure_type,failure_message,log);
    sum_of_fatal++;
    t_failure.fd = fd;
    result = t_failure;
  endfunction

  virtual function void add_error(string error_type, string error_message, string log);
    sv_error t_error = new(error_type, error_message, log);
    sum_of_error++;
    t_error.fd = fd;
    result = t_error;
  endfunction

  virtual function void add_skip();
    sv_skip t_skip = new();
    t_skip.fd = fd;
    result = t_skip;
  endfunction

  virtual function void display();
    $fdisplay( fd, "\t\t<testcase classname=\"%s\" name=\"%s\" time=\"0\">", classname, name );
      if ( result != null ) begin
        result.display();
      end
    $fdisplay( fd, "\t\t</testcase>");
  endfunction

endclass

class sv_property;
  integer fd;
endclass

class sv_testsuit;
  integer fd;
  sv_testcase t_case[$];
  sv_testcase tc;

  string name;

  int failers;
  int errors;

  function new();

  endfunction

  virtual function void display();
    int size = t_case.size();

    $fdisplay( fd, "\t<testsuite name=\"%s\" tests=\"%d\" errors=\"%d\" failures=\"%d\" time=\"0.003\">", name, size, errors, failers );

    for( int i=0; i<size; i++ ) begin
      sv_testcase c = t_case.pop_front();
      c.display();
    end
    $fdisplay( fd, "\t</testsuite>");
  endfunction

  virtual function sv_testcase create_testcase();
    tc = new();
    tc.fd = fd;

    t_case.push_back(tc);

    create_testcase=tc;
  endfunction

endclass

class sv_unit;
  integer fd;

  sv_testsuit t_suit[$];
  sv_testsuit ts;

  function new();

  endfunction

  virtual function void init(string filename);
    fd = $fopen(filename, "w+");

  endfunction
  virtual function sv_testsuit create_testsuit();
    ts = new();
    ts.fd = fd;

    t_suit.push_back(ts);

    create_testsuit = ts;
  endfunction

  virtual function void display();
    int size = t_suit.size();

    $fdisplay( fd, "<?xml version=\"1.0\" ?>");
    $fdisplay( fd, "<testsuites>");
    for( int i=0; i< size; i++ ) begin
      sv_testsuit t = t_suit.pop_front();
      t.display();
    end
    $fdisplay( fd, "</testsuites>");
    $fdisplay( fd, "");

  endfunction

  virtual function void close();
    $fclose( fd );
  endfunction

endclass

`define SV_INIT(SVU) \
SVU.init("sv_all_tests.xml"); \
SVU.create_testsuit(); \
SVU.ts.name = "testsuit_1";

`define SV_TESTCASE(SVU,CLASSNAME,NAME) \
SVU.ts.create_testcase(); \
SVU.ts.tc.setName(CLASSNAME,NAME);

`define SV_TEST_SKIP(SVU) \
SVU.ts.tc.add_skip();

`define SV_TEST_ERROR(SVU,TYPE,MESSAGE,LOG) \
SVU.ts.errors++; \
SVU.ts.tc.add_error(TYPE,MESSAGE,LOG);

`define SV_TEST_FAILER(SVU,TYPE,MESSAGE,LOG) \
SVU.ts.failers++; \
SVU.ts.tc.add_failure(TYPE,MESSAGE,LOG);

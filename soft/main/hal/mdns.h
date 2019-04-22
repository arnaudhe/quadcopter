#pragma once

#include <string>

using namespace std;

class Mdns
{
  private:

  public:

    Mdns(string hostname, string instance_name);

    void add_service(string name, string protocol, int port);

};
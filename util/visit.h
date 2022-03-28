#ifndef ITMO_PARSAT_VISIT_H
#define ITMO_PARSAT_VISIT_H

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif  // ITMO_PARSAT_VISIT_H

# Threat model

## What this project does and where untrusted input enters
The MongoDB database server (C++). Untrusted input: the wire protocol (OP_MSG/OP_QUERY), BSON
documents, commands, replication/replica-set messages, and gridFS uploads. The BSON parser and
command dispatch are the core untrusted surfaces.

## Components that matter most / least
- most: the BSON/MessageLayer deserialization, command handlers, and the expression/query
  evaluator.
- less: tooling, tests.

## How to exercise it
Send crafted BSON/OP_MSG documents and commands to a running mongod; use the existing test
drivers. Focus on parsers and the query/expression engine.

## How you rate severity
- Memory-safety bugs (overflow/UAF) in protocol or BSON parsing that can be triggered by a
  remote client: critical (RCE).
- Logic bugs enabling auth bypass or data corruption: high.
- DoS: medium.

## Anything to leave alone
- Do not report issues requiring an authenticated admin command as high severity.

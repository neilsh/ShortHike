//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef GAME_PORT_HPP
#define GAME_PORT_HPP

class Module;

class Port
{
public:
  Port(const Vector& position = Vector::ZERO, const Quaternion& orientation = Quaternion::IDENTITY);
  ~Port() {}

  bool canConnect(Port* otherPort, const Matrix& myTransform, const Matrix& otherTransform);
//   bool canConnect(Port* otherPort);
//   bool canConnect(Port* otherPort, const Vector& module1Position, const Quaternion& module1Orientation,
//                   const Vector& module2Position, const Quaternion& module2Orientation) const;

  const Vector& getPosition() const;
  Vector& getPosition() {return position;}
  const Quaternion& getOrientation() const;
  Quaternion& getOrientation() {return orientation;}
  Matrix getTransform() const;
  void setPosition(const Vector& newPosition);
  void setOrientation(const Quaternion& newOrientation);
  void setTransform(const Matrix& transform);
private:
//   const string& getName() const;
//   Module* getHost() const;

//   bool isConnected();
//   Port* getConnection() {return mConnection;}
//   void connect(Port* otherPort);
//   void disconnect();


//   Module* mModule;
//   Port* mConnection;  

//   string name;
  
  Vector position;
  Quaternion orientation;
};

#endif

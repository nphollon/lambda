#define BOOST_TEST_MODULE SlalomTest
#include <boost/test/included/unit_test.hpp>

#include "node.hpp"
#include "parse.hpp"
#include "generate.hpp"
#include "tester.hpp"
#include "mock.hpp"

ModuleWriterMock writeToMock(const string& program) {
  ModuleWriterMock mock;
  generate(mock, parse(program));
  return mock;
}


void testMock(Tester& tester) {
  ModuleWriterMock mock;
  tester.verify(!mock.didCreateI(), "Expected mock.createICombinator() to not be called");
  mock.createICombinator();
  tester.verify(mock.didCreateI(), "Expected mock.createICombinator() to be called");
}

BOOST_AUTO_TEST_SUITE( test_generator )

BOOST_AUTO_TEST_CASE( writer_should_create_i_when_program_is_i ) {
  ModuleWriterMock writer = writeToMock("I");
  BOOST_CHECK( writer.didCreateI() );
}

BOOST_AUTO_TEST_SUITE_END()


struct NameNodeFixture {
  NodeFactory factory;
  const Node *aNode;

  NameNodeFixture() {
    aNode = factory.buildNode("A");
  }
  ~NameNodeFixture() {
    factory.deleteNodes();
  }
};

BOOST_FIXTURE_TEST_SUITE( test_name_node, NameNodeFixture )

BOOST_AUTO_TEST_CASE( node_should_equal_node_with_same_name ) {
  const Node *identicalNode = factory.buildNode("A");
  BOOST_CHECK( *aNode == *identicalNode );
  BOOST_CHECK( *identicalNode == *aNode );
  BOOST_CHECK( !(*aNode != *identicalNode) );
}

BOOST_AUTO_TEST_CASE( node_should_not_equal_node_with_different_name ) {
  const Node *differentNode = factory.buildNode("B");
  BOOST_CHECK( !(*aNode == *differentNode) );
  BOOST_CHECK( *aNode != *differentNode );
  BOOST_CHECK( *differentNode != *aNode );
}

BOOST_AUTO_TEST_CASE( name_node_should_be_terminal ) {
  BOOST_CHECK( aNode->isTerminal() );
  BOOST_CHECK( !aNode->getApplicator() );
  BOOST_CHECK( !aNode->getInput() );
}

BOOST_AUTO_TEST_CASE( getName_on_name_node_should_return_name ) {
  BOOST_CHECK_EQUAL( aNode->getName(), "A" );
}

BOOST_AUTO_TEST_CASE( name_node_should_be_immutable ) {
  std::string mutatedName = aNode->getName();
  mutatedName += "more stuff";
  BOOST_CHECK_EQUAL( aNode->getName(), "A" );
}

BOOST_AUTO_TEST_SUITE_END()


struct ApplyNodeFixture {
  NodeFactory factory;
  const Node *childA;
  const Node *childB;
  const Node *parentAB;

  ApplyNodeFixture() {
    childA = factory.buildNode("A");
    childB = factory.buildNode("B");
    parentAB = factory.buildNode(*childA, *childB);
  }
  ~ApplyNodeFixture() {
    factory.deleteNodes();
  }
};

BOOST_FIXTURE_TEST_SUITE( test_apply_node, ApplyNodeFixture )

BOOST_AUTO_TEST_CASE( getName_on_apply_node_should_combine_names_of_children ) {
  BOOST_CHECK_EQUAL( parentAB->getName(), "(A B)" );

  const Node *parentBA = factory.buildNode(*childB, *childA);
  BOOST_CHECK_EQUAL( parentBA->getName(), "(B A)" );
}

BOOST_AUTO_TEST_CASE( apply_node_should_not_be_terminal ) {
  BOOST_CHECK( !parentAB->isTerminal() );
}

BOOST_AUTO_TEST_CASE( apply_node_applicator_should_be_first_child ) {
  BOOST_CHECK_EQUAL( *parentAB->getApplicator(), *childA );
}

BOOST_AUTO_TEST_CASE( apply_node_input_should_be_second_child ) {
  BOOST_CHECK_EQUAL( *parentAB->getInput(), *childB );
}

BOOST_AUTO_TEST_CASE( apply_node_should_not_equal_name_node ) {
  const Node *childless = factory.buildNode("(A B)");
  BOOST_CHECK_NE( *childless, *parentAB );
  BOOST_CHECK_NE( *parentAB, *childless );
}

BOOST_AUTO_TEST_CASE( apply_node_should_check_equality_of_children ) {
  // This test ensures that the node equality operator does a deep comparison,
  // not merely comparing results of getName()
  const Node *childless = factory.buildNode("(A B)");
  const Node *grandparentABC = factory.buildNode(*parentAB, *childless);
  const Node *grandparentCAB = factory.buildNode(*childless, *parentAB);
  BOOST_CHECK_NE( *grandparentABC, *grandparentCAB );
}

BOOST_AUTO_TEST_CASE( copy_constructor_should_create_deep_copy ) {
  const Node *copy = new Node(*parentAB);
  factory.deleteNodes(); // deletes childA, childB, and parentAB

  BOOST_CHECK_EQUAL( copy->getName(), "(A B)" );
  BOOST_CHECK_EQUAL( *copy->getApplicator(), *factory.buildNode("A") );
  BOOST_CHECK_EQUAL( *copy->getInput(), *factory.buildNode("B") );

  delete copy;
}

BOOST_AUTO_TEST_SUITE_END()


struct ParserFixture {
  NodeFactory factory;
  const Node *nodeA;
  const Node *nodeB;

  ParserFixture() {
    nodeA = factory.buildNode("Aa");
    nodeB = factory.buildNode("Bb");
  }

  ~ParserFixture() {
    factory.deleteNodes();
  }

  void verifyParse(const std::string& program, const Node *expectedParseTree) {
    const Node *actualParseTree = parse(program);
    BOOST_CHECK_EQUAL(*actualParseTree, *expectedParseTree);
    delete actualParseTree;
  }
};

void testParser(Tester& tester, const NodeFactory& factory) {  
  { // Test parsing empty program
    const Node *expected = factory.buildNode("I");
    tester.verifyParse("", expected);
    factory.deleteNodes();
  }
  
  { // Test parsing single node
    const Node *nodeA = factory.buildNode("A");
    const Node *nodeB = factory.buildNode("B");
    const Node *nodeNode = factory.buildNode("Node");
    tester.verifyParse("A", nodeA);
    tester.verifyParse("B", nodeB);
    tester.verifyParse("Node", nodeNode);
    factory.deleteNodes();
  }
  
  { // Test parsing two nodes
    const Node *nodeA = factory.buildNode("Aa");
    const Node *nodeB = factory.buildNode("B");
    const Node *nodeAB = factory.buildNode(*nodeA, *nodeB);
    const Node *nodeBA = factory.buildNode(*nodeB, *nodeA);
    tester.verifyParse("Aa B", nodeAB);
    tester.verifyParse("B Aa", nodeBA);
    factory.deleteNodes();
  }
  
  { // Test parsing three nodes sans parentheses
    const Node *nodeA = factory.buildNode("Aa");
    const Node *nodeB = factory.buildNode("Bb");
    const Node *nodeAB = factory.buildNode(*nodeA, *nodeB);
    const Node *nodeC = factory.buildNode("Cc");
    const Node *nodeABC = factory.buildNode(*nodeAB, *nodeC);
    tester.verifyParse("Aa Bb Cc", nodeABC);
    factory.deleteNodes();
  }

  { // Test parsing three nodes with first two nodes grouped
    const Node *nodeA = factory.buildNode("Aa");
    const Node *nodeB = factory.buildNode("Bb");
    const Node *nodeAB = factory.buildNode(*nodeA, *nodeB);
    const Node *nodeC = factory.buildNode("Cc");
    const Node *nodeABC = factory.buildNode(*nodeAB, *nodeC);
    tester.verifyParse("(Aa Bb) Cc", nodeABC);
    factory.deleteNodes();
  }

  { // Test parsing three nodes with last two nodes grouped
    const Node *nodeA = factory.buildNode("Aa");
    const Node *nodeB = factory.buildNode("Bb");
    const Node *nodeC = factory.buildNode("Cc");
    const Node *nodeBC = factory.buildNode(*nodeB, *nodeC);
    const Node *nodeABC = factory.buildNode(*nodeA, *nodeBC);
    tester.verifyParse("Aa (Bb Cc)", nodeABC);
    factory.deleteNodes();
  }

  { // Test parsing four nodes with pairs grouped
    const Node *nodeA = factory.buildNode("Aa");
    const Node *nodeB = factory.buildNode("Bb");
    const Node *nodeC = factory.buildNode("Cc");
    const Node *nodeD = factory.buildNode("Dd");

    const Node *nodeAB = factory.buildNode(*nodeA, *nodeB);
    const Node *nodeCD = factory.buildNode(*nodeC, *nodeD);
    
    const Node *nodeABCD = factory.buildNode(*nodeAB, *nodeCD);
    tester.verifyParse("(Aa Bb) (Cc Dd)", nodeABCD);
    factory.deleteNodes();
  }

  { // Test parsing with unusual whitespace and parens
    const Node *nodeI = factory.buildNode("I");
    const Node *nodeA = factory.buildNode("A");
    const Node *nodeB = factory.buildNode("B");
    const Node *nodeAB = factory.buildNode(*nodeA, *nodeB);
    tester.verifyParse("()", nodeI);
    tester.verifyParse(" ", nodeI);
    tester.verifyParse(" A", nodeA);
    tester.verifyParse("A ", nodeA);
    tester.verifyParse("\tA", nodeA);
    tester.verifyParse("( A )", nodeA);
    tester.verifyParse("A\tB", nodeAB);
    tester.verifyParse("A ((B))", nodeAB);
    tester.verifyParse("(A)(B)", nodeAB);
    tester.verifyParse("A(B)", nodeAB);
    tester.verifyParse("(A)B", nodeAB);
    tester.verifyParse("(\t A  )\t\t( ((B)  )\t)", nodeAB);
    factory.deleteNodes();
  }

  { // Test parsing erroneous programs
    tester.verifyParseError("(");
    tester.verifyParseError(") (");
  }
}

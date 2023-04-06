#define BUCKET_ALLOCATOR_IMPLEMENTATION
#include "bucket.h"
#include "logic_gate.h"
#include <stdio.h>
#include <unistd.h>

SimulationState state;

TruthTable BusTruthTable;
TruthTable OrGateTruthTable;
TruthTable NotGateTruthTable;

int main (void) {

    state.gateAllocator            = bucket_allocator_new(sizeof(Gate));
    state.gateConnectionAllocator  = bucket_allocator_new(sizeof(GateConnectionNode));
    state.logicComponentAllocator  = bucket_allocator_new(sizeof(LogicComponent));
    state.uint64ListItemAllocator  = bucket_allocator_new(sizeof(UInt64ListItem));
    
    gate_init_bus_truth_table(BusTruthTable);
    gate_init_or_truth_table(OrGateTruthTable);
    gate_init_not_truth_table(NotGateTruthTable);

    state.defaultBusTruthTable = BusTruthTable;
    state.defaultOrTruthTable = OrGateTruthTable;
    state.defaultNotTruthTable = NotGateTruthTable;

    LogicComponent * and = logic_component_new(&state);
    Gate * not = gate_new(&state, "not", 3, 3, NotGateTruthTable);
    Gate * or  = gate_new(&state, "or",  2, 1, OrGateTruthTable);

    logic_component_connections_start(&state, and);
    logic_component_gate_connect(&state, and, and->inputBus, GATE_OUTPUT_1, not,             GATE_INPUT_1);
    logic_component_gate_connect(&state, and, and->inputBus, GATE_OUTPUT_2, not,             GATE_INPUT_2);
    logic_component_gate_connect(&state, and, not,           GATE_OUTPUT_1, or,              GATE_INPUT_1);
    logic_component_gate_connect(&state, and, not,           GATE_OUTPUT_2, or,              GATE_INPUT_2);
    logic_component_gate_connect(&state, and, or,            GATE_OUTPUT_1, not,             GATE_INPUT_3);
    logic_component_gate_connect(&state, and, not,           GATE_OUTPUT_3, and->outputBus,  GATE_INPUT_1);
    logic_component_connections_end(&state, and);

    and->inputBus->inputValue = 0b00000011;
    printf("Initial Inputs: "BYTES_1_TO_BINARY_PATTERN"\n", BYTES_1_TO_BINARY(and->inputBus->inputValue));
    logic_component_process(&state, and);
    printf("Resulting Output: "BYTES_1_TO_BINARY_PATTERN"\n", BYTES_1_TO_BINARY(TT_IO_Value(GATE_OUTPUT_1, and->outputBus->outputValue)));

}
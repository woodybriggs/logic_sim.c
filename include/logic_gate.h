#ifndef LOGIC_GATE_H
#define LOGIC_GATE_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bucket.h"
#include "uint64_array.h"
#include "uint64_list.h"

#define BYTES_1_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTES_1_TO_BINARY(byte) \
    ((byte)&0x80 ? '1' : '0'),  \
    ((byte)&0x40 ? '1' : '0'),  \
    ((byte)&0x20 ? '1' : '0'),  \
    ((byte)&0x10 ? '1' : '0'),  \
    ((byte)&0x08 ? '1' : '0'),  \
    ((byte)&0x04 ? '1' : '0'),  \
    ((byte)&0x02 ? '1' : '0'),  \
    ((byte)&0x01 ? '1' : '0')

#define TT_IO_Field(ioNumber, value) (value << (ioNumber))
#define TT_IO_Field_Mask(ioNumber) (1 << (ioNumber))
#define TT_IO_Value(ioNumber, value) ((TT_IO_Field_Mask(ioNumber) & value) >> (ioNumber))

#define GATE_INPUT_1 0
#define GATE_INPUT_2 1
#define GATE_INPUT_3 2
#define GATE_INPUT_4 3
#define GATE_INPUT_5 4
#define GATE_INPUT_6 5
#define GATE_INPUT_7 6
#define GATE_INPUT_8 7
#define GATE_INPUT_MAX 8

#define GATE_OUTPUT_1 0
#define GATE_OUTPUT_2 1
#define GATE_OUTPUT_3 2
#define GATE_OUTPUT_4 3
#define GATE_OUTPUT_5 4
#define GATE_OUTPUT_6 5
#define GATE_OUTPUT_7 6
#define GATE_OUTPUT_8 7
#define GATE_OUTPUT_MAX 8

typedef uint8_t  uint8;
typedef uint64_t uint64;

typedef struct GateConnectionNode GateConnectionNode;
typedef struct Gate Gate;
typedef uint8_t TruthTable[256];

struct GateConnectionNode {
    uint64 sourceGateId;
    Gate * sourceGate;
    uint8 sourceOutputField;
    bool isOccupied;
    GateConnectionNode * next;
};

struct Gate {
    const char *gateName;
    uint64 gateId;
    uint8 numInputs;
    uint8 numOutputs;
    uint8 inputValue;
    uint8 outputValue;
    uint8 * truthTable;
    GateConnectionNode * inputConnections[GATE_INPUT_MAX];
};

typedef struct SimulationState_T {
    BucketAllocator * gateAllocator;
    BucketAllocator * gateConnectionAllocator;
    BucketAllocator * logicComponentAllocator;
    BucketAllocator * uint64ListItemAllocator;

    uint8_t *defaultBusTruthTable;
    uint8_t *defaultOrTruthTable;
    uint8_t *defaultNotTruthTable;
} SimulationState;

typedef struct LogicComponent_T {
    Gate * inputBus;
    Gate * outputBus;
    UInt64List gateEvaluationList;
    uint64_t lastAddedSource;
    uint64_t lastDestination;
} LogicComponent;

Gate * gate_new(
    SimulationState * state,
    const char * gateName,
    uint8 numInputs,
    uint8 numOutputs,
    TruthTable truthTable
) {
    uint64 gateId = state->gateAllocator->length;
    Gate * gate = (Gate *) bucket_allocator_allocate(state->gateAllocator);
    gate->gateName = gateName;
    gate->gateId = gateId;
    gate->inputValue = 0;
    gate->outputValue = 0;
    gate->numInputs = numInputs;
    gate->numOutputs = numOutputs;
    gate->truthTable = truthTable;
    gate->inputConnections[GATE_INPUT_1] = (GateConnectionNode *) bucket_allocator_allocate(state->gateConnectionAllocator);
    gate->inputConnections[GATE_INPUT_2] = (GateConnectionNode *) bucket_allocator_allocate(state->gateConnectionAllocator);
    gate->inputConnections[GATE_INPUT_3] = (GateConnectionNode *) bucket_allocator_allocate(state->gateConnectionAllocator);
    gate->inputConnections[GATE_INPUT_4] = (GateConnectionNode *) bucket_allocator_allocate(state->gateConnectionAllocator);
    gate->inputConnections[GATE_INPUT_5] = (GateConnectionNode *) bucket_allocator_allocate(state->gateConnectionAllocator);
    gate->inputConnections[GATE_INPUT_6] = (GateConnectionNode *) bucket_allocator_allocate(state->gateConnectionAllocator);
    gate->inputConnections[GATE_INPUT_7] = (GateConnectionNode *) bucket_allocator_allocate(state->gateConnectionAllocator);
    gate->inputConnections[GATE_INPUT_8] = (GateConnectionNode *) bucket_allocator_allocate(state->gateConnectionAllocator);
    return gate;
}

void gate_init_or_truth_table(TruthTable truthTable) {
    for (uint64_t input = 0b00000000; input <= 0b11111111; ++input) {
        uint8 output = 0b00000000;

        uint8 gateAOutput = (input & 0b00000011) > 0 ? 0b00000001 : 0b00000000;
        uint8 gateBOutput = (input & 0b00001100) > 0 ? 0b00000010 : 0b00000000;
        uint8 gateCOutput = (input & 0b00110000) > 0 ? 0b00000100 : 0b00000000;
        uint8 gateDOutput = (input & 0b11000000) > 0 ? 0b00001000 : 0b00000000;
        
        output = gateAOutput | gateBOutput | gateCOutput | gateDOutput;
        truthTable[input] = output;
    }
}

void gate_init_not_truth_table(TruthTable truthTable) {
    for (uint64_t input = 0b00000000; input <= 0b11111111; ++input) {
        truthTable[input] = UINT8_MAX-input;
    }
}

void gate_init_bus_truth_table(TruthTable truthTable) {
    for (uint64_t input = 0b00000000; input <= 0b11111111; input++) {
        truthTable[input] = input;
    }
}

void gate_collect_inputs(Gate * gate) {
    uint8 result = gate->inputValue;
    for (int inputFieldId = 0; inputFieldId < gate->numInputs; ++inputFieldId) {
        GateConnectionNode* connection = gate->inputConnections[inputFieldId];
        
        uint8 fieldValue = 0;
        while (connection && connection->isOccupied) {
            Gate* sourceGate = connection->sourceGate;
            uint8 sourceOutputField = connection->sourceOutputField;

            /*
             * We use a logical OR here to simulate the connection
             * of two wires connecting with eachother
             */
            fieldValue |= (TT_IO_Value(sourceOutputField, sourceGate->outputValue) << inputFieldId);
            connection = connection->next;
        }
        /*
        * We use a logical OR here to simulate the connection
        * of two wires connecting with eachother
        */
        result |= fieldValue;
    }
    
    gate->inputValue = result;
}

void gate_evaluate_inputs(Gate *gate) {
    uint8 inputValue = gate->inputValue;
    uint8 result = gate->truthTable[inputValue];
    gate->outputValue = result;
}

LogicComponent * logic_component_new(SimulationState * state) {
    LogicComponent * result = (LogicComponent *) bucket_allocator_allocate(state->logicComponentAllocator);
    result->gateEvaluationList = list_uint64_new(state->uint64ListItemAllocator);
    result->inputBus =  (Gate *) gate_new(state, "inputBus",  8, 8, state->defaultBusTruthTable);
    result->outputBus = (Gate *) gate_new(state, "outputBus", 8, 8, state->defaultBusTruthTable);

    result->lastAddedSource = UINT64_MAX;
    result->lastDestination = UINT64_MAX;

    return result;
}

void logic_component_connections_start(SimulationState * state, LogicComponent * component) {
    component->lastAddedSource = UINT64_MAX;
}

void logic_component_connections_end(SimulationState * state, LogicComponent * component) {
    list_uint64_append(&component->gateEvaluationList, component->lastDestination);
    component->lastAddedSource = UINT64_MAX;
    component->lastDestination = UINT64_MAX;
}

void logic_component_gate_connect(
    SimulationState * state, 
    LogicComponent * component,
    Gate *sourceGate,
    uint8 sourceOutputField,
    Gate *destinationGate,
    uint8 destinationInputField
) {
    GateConnectionNode * node = destinationGate->inputConnections[destinationInputField];
    while (node && !node->isOccupied) {
        if (node->next) node = node->next;
        else break;
    }
    if (!node) {
        node = (GateConnectionNode *) bucket_allocator_allocate(state->gateConnectionAllocator);
        destinationGate->inputConnections[destinationInputField] = node;
    }
    
    node->sourceGate        = sourceGate;
    node->sourceGateId      = sourceGate->gateId;
    node->sourceOutputField = sourceOutputField;
    node->next              = 0;
    node->isOccupied        = true;

    if (component->lastAddedSource != sourceGate->gateId) {
        list_uint64_append(&component->gateEvaluationList, sourceGate->gateId);
        component->lastAddedSource = sourceGate->gateId;
    }

    component->lastDestination = destinationGate->gateId;
}

void logic_component_process(SimulationState * state, LogicComponent * component) {
    list_uint64_iterator_init(&component->gateEvaluationList);
    for (uint64 i = 0; i < component->gateEvaluationList.length; ++i) {
        UInt64ListItem * gateIdItem = list_uint64_iterator_next(&component->gateEvaluationList);
        Gate * gate = (Gate *) bucket_allocator_get_item_by_index(state->gateAllocator, gateIdItem->data);
        gate_collect_inputs(gate);
        gate_evaluate_inputs(gate);
    }
}

#endif
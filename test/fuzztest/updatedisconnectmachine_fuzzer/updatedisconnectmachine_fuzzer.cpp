/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "updatedisconnectmachine_fuzzer.h"

#define private public
#define protected public

#include "adddatatoken_fuzzer.h"
#include "default.h"
#include "disconnecting.h"
#include "statemachine_fuzzer.h"

namespace OHOS {
using namespace OHOS::Telephony;
using namespace AppExecFwk;
using namespace OHOS::EventFwk;
static int32_t SIM_COUNT = 2;
static int32_t REMAIN = 2;
static int32_t NUM = 10;

void UpdateActiveMachineFuzz(const uint8_t *data, size_t size)
{
    std::shared_ptr<StateMachineFuzzer> machine = std::make_shared<StateMachineFuzzer>();
    if (machine == nullptr) {
        return;
    }
    int32_t slotId = static_cast<int32_t>(size % SIM_COUNT);
    std::shared_ptr<CellularDataStateMachine> cellularMachine = machine->CreateCellularDataConnect(slotId);
    if (cellularMachine == nullptr) {
        return;
    }
    sptr<Disconnecting> disconnecting =
        std::make_unique<Disconnecting>(std::weak_ptr<CellularDataStateMachine>(cellularMachine), "Disconnecting")
            .release();
    sptr<Default> defaultStatus =
        std::make_unique<Default>(std::weak_ptr<CellularDataStateMachine>(cellularMachine), "Default").release();

    if (disconnecting == nullptr || defaultStatus == nullptr) {
        TELEPHONY_LOGE("memory allocation failed");
        return;
    }
    cellularMachine->Init();

    std::int32_t intValue = static_cast<int32_t>(size);
    std::unique_ptr<uint8_t> object = std::make_unique<uint8_t>(*data);
    if (object == nullptr) {
        return;
    }

    if (intValue % NUM == REMAIN) {
        InnerEvent::Pointer event = InnerEvent::Get(intValue, object);
        disconnecting->StateProcess(event);
        disconnecting->ProcessDisconnectTimeout(event);

        defaultStatus->StateProcess(event);
        defaultStatus->ProcessConnectDone(event);
        defaultStatus->ProcessDisconnectAllDone(event);
        defaultStatus->ProcessDataConnectionDrsOrRatChanged(event);
        defaultStatus->ProcessDataConnectionRoamOn(event);
        defaultStatus->ProcessDataConnectionRoamOff(event);
    }

    if (machine->stateMachineEventLoop_ != nullptr) {
        machine->stateMachineEventLoop_->Stop();
    }
}

void UpdateActiveMachineWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    UpdateActiveMachineFuzz(data, size);
    return;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddDataTokenFuzzer token;
    /* Run your code on data */
    OHOS::UpdateActiveMachineWithMyAPI(data, size);
    return 0;
}
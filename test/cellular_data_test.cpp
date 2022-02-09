/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include <string>

#include "gtest/gtest.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "cellular_data_error.h"
#include "cellular_data_types.h"
#include "core_service_client.h"
#include "i_cellular_data_manager.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class CellularDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp();
    virtual void TearDown();
    static int32_t IsCellularDataEnabledTest();
    static int32_t EnableCellularDataTest(bool enable);
    static int32_t GetCellularDataStateTest();
    static int32_t IsCellularDataRoamingEnabledTest(int32_t slotId);
    static int32_t EnableCellularDataRoamingTest(int32_t slotId, bool enable);
    static int32_t GetDefaultCellularDataSlotIdTest();
    static int32_t SetDefaultCellularDataSlotIdTest(int32_t slotId);
    static int32_t GetCellularDataFlowTypeTest();
    static void WaitTestTimeout(const int32_t status);
    static sptr<ICellularDataManager> GetProxy();

public:
    static sptr<ICellularDataManager> proxy_;
    static const int32_t SLEEP_TIME = 1;
    static const int32_t DATA_SLOT_ID_INVALID = DEFAULT_SIM_SLOT_ID + 10;
};

sptr<ICellularDataManager> CellularDataTest::proxy_;

void CellularDataTest::TearDownTestCase()
{}

void CellularDataTest::SetUp()
{}

void CellularDataTest::TearDown()
{}

void CellularDataTest::SetUpTestCase()
{
    if (CoreServiceClient::GetInstance().GetProxy() == nullptr) {
        std::cout << "connect coreService server failed!" << std::endl;
        return;
    }

    proxy_ = GetProxy();
    ASSERT_TRUE(proxy_ != nullptr);
    if (!CoreServiceClient::GetInstance().HasSimCard(DEFAULT_SIM_SLOT_ID)) {
        return;
    }
    // Set the default slot
    int32_t result = proxy_->SetDefaultCellularDataSlotId(DEFAULT_SIM_SLOT_ID);
    if (result != static_cast<int32_t>(DataRespondCode::SET_SUCCESS)) {
        return;
    }
    int32_t enable = proxy_->EnableCellularData(true);
    ASSERT_TRUE(enable == static_cast<int32_t>(DataRespondCode::SET_SUCCESS));
    WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_CONNECTED));
}

void CellularDataTest::WaitTestTimeout(const int32_t status)
{
    if (proxy_ == nullptr) {
        return;
    }
    const int32_t maxTimes = 35;
    int32_t count = 0;
    while (count < maxTimes) {
        sleep(SLEEP_TIME);
        if (proxy_->GetCellularDataState() == status) {
            return;
        }
        count++;
    }
}

int32_t CellularDataTest::IsCellularDataRoamingEnabledTest(int32_t slotId)
{
    if (proxy_ == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = proxy_->IsCellularDataRoamingEnabled(slotId);
    return result;
}

int32_t CellularDataTest::IsCellularDataEnabledTest()
{
    if (proxy_ == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = proxy_->IsCellularDataEnabled();
    return result;
}

int32_t CellularDataTest::EnableCellularDataTest(bool enable)
{
    if (proxy_ == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = proxy_->EnableCellularData(enable);
    return result;
}

int32_t CellularDataTest::GetCellularDataStateTest()
{
    if (proxy_ == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = proxy_->GetCellularDataState();
    return result;
}

int32_t CellularDataTest::EnableCellularDataRoamingTest(int32_t slotId, bool enable)
{
    if (proxy_ == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return proxy_->EnableCellularDataRoaming(slotId, enable);
}

int32_t CellularDataTest::GetDefaultCellularDataSlotIdTest()
{
    if (proxy_ == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return proxy_->GetDefaultCellularDataSlotId();
}

int32_t CellularDataTest::SetDefaultCellularDataSlotIdTest(int32_t slotId)
{
    if (proxy_ == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return proxy_->SetDefaultCellularDataSlotId(slotId);
}

int32_t CellularDataTest::GetCellularDataFlowTypeTest()
{
    if (proxy_ == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return proxy_->GetCellularDataFlowType();
}

sptr<ICellularDataManager> CellularDataTest::GetProxy()
{
    sptr<ISystemAbilityManager> systemAbilityMgr =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        return nullptr;
    }
    sptr<IRemoteObject> remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_DATA_SYS_ABILITY_ID);
    if (remote) {
        sptr<ICellularDataManager> dataManager = iface_cast<ICellularDataManager>(remote);
        return dataManager;
    }
    return nullptr;
}

HWTEST_F(CellularDataTest, GetProxy_Test, TestSize.Level1)
{
    CellularDataTest::proxy_ = CellularDataTest::GetProxy();
    ASSERT_FALSE(CellularDataTest::proxy_ == nullptr);
}

HWTEST_F(CellularDataTest, IsCellularDataEnabled_Test, TestSize.Level1)
{
    int32_t result = CellularDataTest::IsCellularDataEnabledTest();
    ASSERT_TRUE(result >= static_cast<int32_t>(DataSwitchCode::CELLULAR_DATA_DISABLED));
}

HWTEST_F(CellularDataTest, DefaultCellularDataSlotId_Test, TestSize.Level2)
{
    if (!CoreServiceClient::GetInstance().HasSimCard(DEFAULT_SIM_SLOT_ID)) {
        return;
    }
    int32_t result = CellularDataTest::GetDefaultCellularDataSlotIdTest();
    if (result < DEFAULT_SIM_SLOT_ID) {
        return;
    }
    result = CellularDataTest::SetDefaultCellularDataSlotIdTest(DEFAULT_SIM_SLOT_ID);
    ASSERT_TRUE(result == static_cast<int32_t>(DataRespondCode::SET_SUCCESS));
    // Multiple cards will need to be optimized again
    result = CellularDataTest::SetDefaultCellularDataSlotIdTest(DEFAULT_SIM_SLOT_ID - 1);
    ASSERT_TRUE(result == CELLULAR_DATA_INVALID_PARAM);
    result = CellularDataTest::SetDefaultCellularDataSlotIdTest(DATA_SLOT_ID_INVALID);
    ASSERT_TRUE(result == CELLULAR_DATA_INVALID_PARAM);
}

HWTEST_F(CellularDataTest, EnableCellularData_Test, TestSize.Level2)
{
    if (!CoreServiceClient::GetInstance().HasSimCard(DEFAULT_SIM_SLOT_ID)) {
        return;
    }
    int32_t enabled = CellularDataTest::IsCellularDataEnabledTest();
    if (enabled == static_cast<int32_t>(DataSwitchCode::CELLULAR_DATA_ENABLED)) {
        int32_t disabled = CellularDataTest::EnableCellularDataTest(false);
        ASSERT_TRUE(disabled == static_cast<int32_t>(DataRespondCode::SET_SUCCESS));
        WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_DISCONNECTED));
    } else {
        int32_t result = CellularDataTest::EnableCellularDataTest(true);
        ASSERT_TRUE(result == static_cast<int32_t>(DataRespondCode::SET_SUCCESS));
        WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_CONNECTED));
        CellularDataTest::EnableCellularDataTest(false);
        WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_DISCONNECTED));
    }
}

HWTEST_F(CellularDataTest, DataRoamingState_ValidSlot_Test_01, TestSize.Level3)
{
    if (!CoreServiceClient::GetInstance().HasSimCard(DEFAULT_SIM_SLOT_ID)) {
        return;
    }
    CellularDataTest::SetDefaultCellularDataSlotIdTest(DEFAULT_SIM_SLOT_ID);
    int32_t disabled = CellularDataTest::EnableCellularDataTest(false);
    ASSERT_TRUE(disabled == static_cast<int32_t>(DataRespondCode::SET_SUCCESS));
    WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_DISCONNECTED));

    // slot1 enable data roaming
    int32_t enabled = CellularDataTest::EnableCellularDataRoamingTest(DEFAULT_SIM_SLOT_ID, true);
    ASSERT_TRUE(enabled == static_cast<int32_t>(DataRespondCode::SET_SUCCESS));
    int32_t result = CellularDataTest::IsCellularDataRoamingEnabledTest(DEFAULT_SIM_SLOT_ID);
    ASSERT_TRUE(result == static_cast<int32_t>(RoamingSwitchCode::CELLULAR_DATA_ROAMING_ENABLED));
    // slot1 close
    int32_t enable = CellularDataTest::EnableCellularDataRoamingTest(DEFAULT_SIM_SLOT_ID, false);
    ASSERT_TRUE(enable == static_cast<int32_t>(DataRespondCode::SET_SUCCESS));
    result = CellularDataTest::IsCellularDataRoamingEnabledTest(DEFAULT_SIM_SLOT_ID);
    ASSERT_TRUE(result == static_cast<int32_t>(RoamingSwitchCode::CELLULAR_DATA_ROAMING_DISABLED));

    // At present, multiple card problems, the subsequent need to continue to deal with
    enable = CellularDataTest::EnableCellularDataRoamingTest(DATA_SLOT_ID_INVALID, true);
    ASSERT_TRUE(enable == CELLULAR_DATA_INVALID_PARAM);
    result = CellularDataTest::IsCellularDataRoamingEnabledTest(DATA_SLOT_ID_INVALID);
    ASSERT_TRUE(result == CELLULAR_DATA_INVALID_PARAM);
    enable = CellularDataTest::EnableCellularDataRoamingTest(DATA_SLOT_ID_INVALID, false);
    // At present, multiple card problems, the subsequent need to continue to deal with
    ASSERT_TRUE(enable == CELLULAR_DATA_INVALID_PARAM);
    result = CellularDataTest::IsCellularDataRoamingEnabledTest(DATA_SLOT_ID_INVALID);
    ASSERT_TRUE(result == CELLULAR_DATA_INVALID_PARAM);
}

HWTEST_F(CellularDataTest, EnableCellularDataRoaming_ValidSlot_Test_01, TestSize.Level3)
{
    if (!CoreServiceClient::GetInstance().HasSimCard(DEFAULT_SIM_SLOT_ID)) {
        return;
    }
    CellularDataTest::SetDefaultCellularDataSlotIdTest(DEFAULT_SIM_SLOT_ID);
    int32_t disabled = CellularDataTest::EnableCellularDataTest(false);
    ASSERT_TRUE(disabled == static_cast<int32_t>(DataRespondCode::SET_SUCCESS));
    WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_DISCONNECTED));

    int32_t isDataRoaming = CellularDataTest::IsCellularDataRoamingEnabledTest(DEFAULT_SIM_SLOT_ID);
    if (isDataRoaming == static_cast<int32_t>(DataSwitchCode::CELLULAR_DATA_ENABLED)) {
        int32_t result = CellularDataTest::EnableCellularDataRoamingTest(DEFAULT_SIM_SLOT_ID, false);
        ASSERT_TRUE(result == static_cast<int32_t>(DataRespondCode::SET_SUCCESS));
    } else {
        int32_t result = CellularDataTest::EnableCellularDataRoamingTest(DEFAULT_SIM_SLOT_ID, true);
        ASSERT_TRUE(result == static_cast<int32_t>(DataRespondCode::SET_SUCCESS));
    }
    // At present, multiple card problems, the subsequent need to continue to deal with
    isDataRoaming = CellularDataTest::IsCellularDataRoamingEnabledTest(DEFAULT_SIM_SLOT_ID);
    if (isDataRoaming == static_cast<int32_t>(DataSwitchCode::CELLULAR_DATA_ENABLED)) {
        int32_t result = CellularDataTest::EnableCellularDataRoamingTest(DATA_SLOT_ID_INVALID, false);
        ASSERT_TRUE(result == CELLULAR_DATA_INVALID_PARAM);
    } else {
        int32_t result = CellularDataTest::EnableCellularDataRoamingTest(DATA_SLOT_ID_INVALID, true);
        ASSERT_TRUE(result == CELLULAR_DATA_INVALID_PARAM);
    }
}

HWTEST_F(CellularDataTest, GetCellularDataState_ValidityTest_01, TestSize.Level3)
{
    if (!CoreServiceClient::GetInstance().HasSimCard(DEFAULT_SIM_SLOT_ID)) {
        return;
    }
    CellularDataTest::SetDefaultCellularDataSlotIdTest(DEFAULT_SIM_SLOT_ID);
    int32_t enabled = CellularDataTest::IsCellularDataEnabledTest();
    if (enabled == static_cast<int32_t>(DataSwitchCode::CELLULAR_DATA_ENABLED)) {
        CellularDataTest::EnableCellularDataTest(false);
        WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_DISCONNECTED));
        sleep(SLEEP_TIME);
        CellularDataTest::EnableCellularDataTest(true);
        WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_CONNECTED));
        int32_t result = CellularDataTest::GetCellularDataStateTest();
        ASSERT_TRUE(result == static_cast<int32_t>(DataConnectionStatus::DATA_STATE_CONNECTED));
    } else {
        CellularDataTest::EnableCellularDataTest(true);
        WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_CONNECTED));
        sleep(SLEEP_TIME);
        CellularDataTest::EnableCellularDataTest(false);
        WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_DISCONNECTED));
        int32_t result = CellularDataTest::GetCellularDataStateTest();
        ASSERT_TRUE(result == static_cast<int32_t>(DataConnectionStatus::DATA_STATE_DISCONNECTED));
    }
    CellularDataTest::EnableCellularDataTest(false);
    WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_DISCONNECTED));
}

HWTEST_F(CellularDataTest, DataRoamingState_InValidSlot_Test_01, TestSize.Level3)
{
    if (!CoreServiceClient::GetInstance().HasSimCard(DEFAULT_SIM_SLOT_ID)) {
        return;
    }
    // invalid slot turn on data roaming
    int32_t enable = CellularDataTest::EnableCellularDataRoamingTest(DEFAULT_SIM_SLOT_ID -1, true);
    ASSERT_TRUE(enable == CELLULAR_DATA_INVALID_PARAM);
    int32_t result = CellularDataTest::IsCellularDataRoamingEnabledTest(DEFAULT_SIM_SLOT_ID -1);
    ASSERT_TRUE(result == CELLULAR_DATA_INVALID_PARAM);
    enable = CellularDataTest::EnableCellularDataRoamingTest(DATA_SLOT_ID_INVALID, true);
    ASSERT_TRUE(enable == CELLULAR_DATA_INVALID_PARAM);
    result = CellularDataTest::IsCellularDataRoamingEnabledTest(DATA_SLOT_ID_INVALID);
    ASSERT_TRUE(result == CELLULAR_DATA_INVALID_PARAM);
    // invalid slot disable roaming
    enable = CellularDataTest::EnableCellularDataRoamingTest(DEFAULT_SIM_SLOT_ID -1, false);
    ASSERT_TRUE(enable == CELLULAR_DATA_INVALID_PARAM);
    result = CellularDataTest::IsCellularDataRoamingEnabledTest(DEFAULT_SIM_SLOT_ID -1);
    ASSERT_TRUE(result == CELLULAR_DATA_INVALID_PARAM);
    enable = CellularDataTest::EnableCellularDataRoamingTest(DATA_SLOT_ID_INVALID, false);
    ASSERT_TRUE(enable == CELLULAR_DATA_INVALID_PARAM);
    result = CellularDataTest::IsCellularDataRoamingEnabledTest(DATA_SLOT_ID_INVALID);
    ASSERT_TRUE(result == CELLULAR_DATA_INVALID_PARAM);
}

HWTEST_F(CellularDataTest, DataFlowType_Test_01, TestSize.Level3)
{
    if (!CoreServiceClient::GetInstance().HasSimCard(DEFAULT_SIM_SLOT_ID)) {
        return;
    }
    CellularDataTest::SetDefaultCellularDataSlotIdTest(DEFAULT_SIM_SLOT_ID);
    CellularDataTest::EnableCellularDataTest(false);
    WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_DISCONNECTED));
    sleep(SLEEP_TIME);
    CellularDataTest::EnableCellularDataTest(true);
    WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_CONNECTED));
    int32_t dataFlowType = CellularDataTest::GetCellularDataFlowTypeTest();
    ASSERT_TRUE(dataFlowType >= 0);
    CellularDataTest::EnableCellularDataTest(false);
    WaitTestTimeout(static_cast<int32_t>(DataConnectionStatus::DATA_STATE_DISCONNECTED));
    dataFlowType = CellularDataTest::GetCellularDataFlowTypeTest();
    ASSERT_TRUE(dataFlowType == 0);
}
} // namespace Telephony
} // namespace OHOS
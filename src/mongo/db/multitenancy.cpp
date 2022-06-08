/**
 *    Copyright (C) 2021-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/db/multitenancy.h"

#include "mongo/db/auth/authorization_session.h"
#include "mongo/db/auth/security_token.h"
#include "mongo/db/multitenancy_gen.h"
#include "mongo/db/tenant_id.h"
#include "mongo/logv2/log.h"

#define MONGO_LOGV2_DEFAULT_COMPONENT ::mongo::logv2::LogComponent::kDefault


namespace mongo {

// Holds the tenantId for the operation if it was provided in the request on the $tenant field only
// if the tenantId was not also provided in the security token.
const auto dollarTenantDecoration =
    OperationContext::declareDecoration<boost::optional<mongo::TenantId>>();

boost::optional<TenantId> getActiveTenant(OperationContext* opCtx) {
    auto token = auth::getSecurityToken(opCtx);
    if (!token) {
        return dollarTenantDecoration(opCtx);
    }

    invariant(!dollarTenantDecoration(opCtx));
    return token->getAuthenticatedUser().getTenant();
}

void setDollarTenantOnOpCtx(OperationContext* opCtx, const OpMsg& opMsg) {
    if (!opMsg.validatedTenant || !opMsg.validatedTenant->tenantId()) {
        return;
    }

    if (opMsg.securityToken.nFields() > 0) {
        return;
    }

    dollarTenantDecoration(opCtx) = opMsg.validatedTenant->tenantId();
}

}  // namespace mongo

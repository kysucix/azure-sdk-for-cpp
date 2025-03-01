// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-Licence-Identifier: MIT

#pragma once

#include "models/amqp_message.hpp"
#include "session.hpp"
#include <azure/core/context.hpp>
#include <string>
#include <tuple>
#include <vector>

namespace Azure { namespace Core { namespace Amqp { namespace _detail {
  class ManagementImpl;
}}}} // namespace Azure::Core::Amqp::_detail

namespace Azure { namespace Core { namespace Amqp { namespace _internal {
  enum class ManagementOperationStatus
  {
    Invalid,
    Ok,
    Error,
    FailedBadStatus,
    InstanceClosed
  };

  enum class ManagementOpenStatus
  {
    Invalid,
    Ok,
    Error,
    Cancelled,
  };

  struct ManagementOptions
  {
    /**
     * @brief Expected status code key name.
     *
     * Specifies the key name which will contain the result of the management operation.
     *
     * @remarks AMQP management plane operations return the status code and description in the
     * ApplicationProperties field of the AmqpMessage. By default, the error code property is in
     * a key named "statusCode", this allows a client of the Management object to override the
     * default value with one specific to the management API being called.
     *
     * For more information, see [Service Bus Request
     * Response](https://learn.microsoft.com/azure/service-bus-messaging/service-bus-amqp-request-response#response).
     */
    std::string ExpectedStatusCodeKeyName = "statusCode";

    /**
     * @brief Expected status description key name.
     *
     * Specifies the key name which will contain the description of the result of the management
     * operation.
     *
     * @remarks AMQP management plane operations return the status code and description in the
     * ApplicationProperties field of the AmqpMessage. By default, the error description
     * property is in a key named "statusDescription", this allows a client of the Management
     * object to override the default value with one specific to the management API being
     * called.
     *
     * For more information, see [Service Bus Request
     * Response](https://learn.microsoft.com/azure/service-bus-messaging/service-bus-amqp-request-response#response).
     */
    std::string ExpectedStatusDescriptionKeyName = "statusDescription";

    /**
     * @brief Enable trace logging for the management operations.
     */
    bool EnableTrace{false};
  };

  /**
   * @brief Callback event handler for management events such as error.
   */
  struct ManagementEvents
  {
    /** @brief Called when an error occurs.
     */
    virtual void OnError() = 0;
  };

  /**
   * @brief Result of a management operation.
   */
  struct ManagementOperationResult
  {
    /**
     * @brief The status of the operation.
     */
    ManagementOperationStatus Status = ManagementOperationStatus::Invalid;
    /**
     * @brief The response message from the operation, if Status is ManagementOperationStatus::Ok.
     */
    Models::AmqpMessage Message;

    /**
     * @brief The description of the operation, if Status is ManagementOperationStatus::Error.
     */
    std::string Description;

    /**
     * @brief The HTTP status code of the operation, if Status is ManagementOperationStatus::Error.
     */
    uint32_t StatusCode = 0;
  };

  /** @brief AMQP Management APIs.
   *
   * The AMQP management plane is a set of APIs that allow for management operations to be
   * performed on an AMQP connection. See [AMQP Management
   * Version 1.0](https://www.oasis-open.org/committees/download.php/52425/amqp-man-v1%200-wd08.pdf)
   * for more information.
   *
   */
  class Management final {
  public:
    /**
     * @brief Create a new Management object instance.
     *
     * @param session - the session on which to create the instance.
     * @param managementNodeName - the name of the message source and target.
     * @param options - additional options for the Management object.
     * @param managementEvents - events associated with the management object.
     */
    Management(
        Session const& session,
        std::string const& managementNodeName,
        ManagementOptions const& options,
        ManagementEvents* managementEvents = nullptr);

    Management(std::shared_ptr<Azure::Core::Amqp::_detail::ManagementImpl> impl) : m_impl{impl} {}
    ~Management() noexcept = default;

    /**
     * @brief Open the management instance.
     *
     * @returns The result of the open operation.
     */
    ManagementOpenStatus Open(Azure::Core::Context const& context = {});

    /**
     * @brief Close the management instance.
     */
    void Close();

    /**
     * @brief Execute a management operation.
     *
     * @param operationToPerform - the operation to perform (case sensitive).
     * @param typeOfOperation - the type of operation (case sensitive).
     * @param locales - the locales to use - A list of locales that the sending peer permits for
     * incoming informational text in response messages. This value MUST be of the form presented
     * in the "Language-Tag" rule of [RFC2616], section 3.10.
     * @param messageToSend - the message to send.
     * @param context - the context for the operation.
     *
     * @returns a ManagementOperationResult which includes the high level result of the operation,
     * the HTTP response status code, the status description, and the response message.
     */
    ManagementOperationResult ExecuteOperation(
        std::string const& operationToPerform,
        std::string const& typeOfOperation,
        std::string const& locales,
        Azure::Core::Amqp::Models::AmqpMessage const& messageToSend,
        Azure::Core::Context context = {});

  private:
    std::shared_ptr<Azure::Core::Amqp::_detail::ManagementImpl> m_impl;
  };

}}}} // namespace Azure::Core::Amqp::_internal

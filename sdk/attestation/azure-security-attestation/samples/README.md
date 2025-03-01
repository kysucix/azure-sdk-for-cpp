---
page_type: sample
languages:
- C++
products:
- azure
- azure-attestation
urlFragment: attestation-samples

---

# Samples for the Microsoft Azure Attestation client library for C++

These code samples show common scenario operations with the Azure Attestation client library.

## Service operational modes

As was mentioned in the [README.md file][readme_md], the attestation service
operates in three different modes:

* Shared
* AAD
* Isolated

The core difference between the three modes of operation is the operations which
are permitted on each, and if the customer needs to create an
instance of the provider.

Service Mode | Instance Creation  | Attestation | Policy Get | Policy Set | Signed Policies| Isolated Mode Certificate
------ | --- | ---- | ----  | --- | --- | ---
Shared | No | Yes | Yes (default always)| No | No | No
AAD | Yes | Yes | Yes | Yes | Optional | No
Isolated | Yes| Yes | Yes | Yes | Yes | Yes

### Shared Mode

Each region in which the MAA service operates has a "shared" attestation instance
which allows customers to perform basic attestation operations on their enclaves
without having to set up an attestation instance.
That instance is limited in what actions it can perform: The shared instance has
a "default" attestation policy which simply attests the correctness of the SGX
attestation collateral. It cannot be used for attestation types like `TPM` which
require that the customer provide an attestation policy. However, for customers
who simply need to perform attestation operations on an SGX enclave, they can use
the shared instance without creating their own instance.

Examples of shared instances are:

* sharedeus2.eus2.attest.azure.net
* sharedcae.cae.attest.azure.net
* shareduks.uks.attest.azure.net

### AAD Mode

AAD mode instances are intended for customers who trust ARM RBAC for authorization
decisions related to policy management. Attestation policies are allowed to be
either be signed or unsigned.

### Isolated Mode

Isolated mode instances are intended for customers who desire an additional level
of authorization beyond that which is allowed by ARM RBAC authorization. When a
customer creates an isolated attestation instance, they also need to create an
RSA asymmetric key pair and an X.509 certificate which contains that asymmetric
key (the certificate can be self-signed, or it can be issued by a certificate
authority). Attestation policies MUST be signed with one of the private keys
associated with the instance (either at instance creation or added with the [AddIsolatedModeCertificate][add_isolated_mode_cert] API).

## Sample Requirements

These samples are written with the assumption that the following environment
variables have been set by the user:

* ATTESTATION_AAD_URL - the base URL for an attestation service instance in AAD mode.
* ATTESTATION_ISOLATED_URL - the base URL for an attestation service instance in Isolated mode.
* ATTESTATION_LOCATION_SHORT_NAME - the short name for the region in which the
  sample should be run - used to interact with the shared endpoint for that
  region.
* ISOLATED_SIGNING_CERTIFICATE - The DER encoded form of the signing
  certificate used to create an isolated attestation instance, Base64 encoded.
* ISOLATED_SIGNING_KEY - The DER encoded of an RSA Private key,
  Base64 encoded, which was used to create an isolated attestation service instance.
* POLICY_SIGNING_KEY_0 - The DER encoded of an RSA Private key,
  Base64 encoded.
* POLICY_SIGNING_CERTIFICATE_0 - The DER encoded form of a self-signed
  certificate which contains the public key associated with policySigningKey0, Base64 encoded.

The tests also assume that the currently logged on user is authorized to call
into the attestation service instance because they use [Azure::Core::Credentials::TokenCredential](https://azuresdkdocs.blob.core.windows.net/$web/cpp/azure-core/1.3.1/class_azure_1_1_core_1_1_credentials_1_1_token_credential.html) for authorization.

This directory contains functional samples for the attestation service SDK.

## Samples descriptions

The samples are roughly grouped into four source files, by functionality:

* AttestationSamples - demonstrate the "attestation" family of APIs.
* PolicySamples - demonstrate the "policy" family of APIs, including setting, getting and resetting attestation policies.
* PolicyManagementCertificateSamples - demonstrates the "policy certificates" family of APIs.
* BasicOperations - these are samples which show functions which are not specific to the above families.

Functions | Source Location | Executable Name
-----| ------|----
Attestation | test\samples\attestation | attestation-attestation
Policy | test\samples\policy | attestation-policy
PolicyManagementCertificates | test\samples\policy-management| attestation-policy-management
Basic Operations | test\samples\basic-operations| attestation-basic-operations

## Additional Information

### Attestation Policy

An attestation policy is a document which defines authorization and claim generation
rules for attestation operations.

The following is an example of an attestation policy document for an SGX enclave:

```text
version= 1.0;
authorizationrules
{
    [ type=="x-ms-sgx-is-debuggable", value==false ] &&
    [ type=="x-ms-sgx-product-id", value==<product-id> ] &&
    [ type=="x-ms-sgx-svn", value>= 0 ] &&
    [ type=="x-ms-sgx-mrsigner", value=="<mrsigner>"]
        => permit();
};
issuancerules {
    c:[type=="x-ms-sgx-mrsigner"] => issue(type="<custom-name>", value=c.value);
};
```

There are two sections to the document: `authorizationrules` and `issuancerules`.
`authorizationrules` are rules which control whether an attestation token
should be issued. `issuancerules` are rules which cause claims to be issued in an
attestation token.

In the example, the attestation service will issue an attestation token if and only if
the SGX enclave is configured as follows:

* Not-Debuggable
* Enclave product ID: `<product-id>`.
* Enclave SVN: `<svn value>` greater or equal to zero.
* Enclave signer: matches `<mrsigner>`.

Assuming a token is issued, this policy will cause a claim named `<custom-name>`
to be issued with a value which matches the `x-ms-sgx-mrsigner` claim.

For more information on authoring attestation policy documents, see: [Authoring an attestation policy](https://docs.microsoft.com/azure/attestation/author-sign-policy)

## Next Steps

For more information about the Microsoft Azure Attestation service, please see our [documentation page](https://docs.microsoft.com/azure/attestation/) .

## Contributing

This project welcomes contributions and suggestions. Most contributions require you to agree to a [Contributor License Agreement (CLA)][cla] declaring that you have the right to, and actually do, grant us the rights to use your contribution.

If you'd like to contribute to this library, please read the [contributing guide][contributing_guide] to learn more about how to build and test the code.

When you submit a pull request, a CLA-bot will automatically determine whether you need to provide a CLA and decorate the PR appropriately (e.g., label, comment). Simply follow the instructions provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct][coc]. For more information, see the [Code of Conduct FAQ][coc_faq] or contact [opencode@microsoft.com][coc_contact] with any additional questions or comments.

<!-- LINKS -->
[contributing_guide]: https://github.com/Azure/azure-sdk-for-cpp/blob/main/CONTRIBUTING.md
[cla]: https://cla.microsoft.com
[coc]: https://opensource.microsoft.com/codeofconduct/
[coc_faq]: https://opensource.microsoft.com/codeofconduct/faq/
[coc_contact]: mailto:opencode@microsoft.com
[readme_md]: https://github.com/Azure/azure-sdk-for-cpp/blob/main/sdk/attestation/azure-security-attestation/README.md
[add_isolated_mode_cert]https://azuresdkdocs.blob.core.windows.net/$web/cpp/azure-security-attestation/1.0.0-beta.1/class_azure_1_1_security_1_1_attestation_1_1_attestation_administration_client.html#ad5e9fed7924e3fa89a6b23ba12c5cb35

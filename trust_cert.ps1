$thumb = "8CBA126115C3FA3B8A07967188EC9C5B96E26479"
$cert = Get-Item "Cert:\CurrentUser\My\$thumb"
$storeRoot = New-Object System.Security.Cryptography.X509Certificates.X509Store("Root", "CurrentUser")
$storeRoot.Open("ReadWrite")
$storeRoot.Add($cert)
$storeRoot.Close()

$storePeople = New-Object System.Security.Cryptography.X509Certificates.X509Store("TrustedPeople", "CurrentUser")
$storePeople.Open("ReadWrite")
$storePeople.Add($cert)
$storePeople.Close()

Write-Host "Cert added to CurrentUser Root and TrustedPeople"

param (
    [string]$BuildResult
)

# Set SMTP server settings
$smtpServer = "smtp.gmail.com"
$smtpFrom = "opentwin.buildauto@gmail.com"
$smtpTo = $env:OPEN_TWIN_EMAIL_LIST
$messageSubject = "OpenTwin Nightly Build $BuildResult"
$messageBody = Get-Content -Raw -Path "$env:OPENTWIN_DEV_ROOT\Scripts\BuildAndTest\buildLog_Summary.txt"
$smtpUsername = "opentwin.buildauto@gmail.com"
$smtpPassword = $env:OPEN_TWIN_EMAIL_PWD
$attachmentPath1 = "$env:OPENTWIN_DEV_ROOT\Scripts\BuildAndTest\buildLog_Release.txt"
$attachmentPath2 = "$env:OPENTWIN_DEV_ROOT\Scripts\BuildAndTest\buildLog_Debug.txt"

# Create email message
$message = New-Object system.net.mail.mailmessage
$message.from = $smtpFrom
$message.To.Add($smtpTo)
$message.Subject = $messageSubject
$message.Body = $messageBody
$message.IsBodyHtml = $false

# Add attachment
$attachment1 = New-Object System.Net.Mail.Attachment($attachmentPath1)
$attachment2 = New-Object System.Net.Mail.Attachment($attachmentPath2)
$message.Attachments.Add($attachment1)
$message.Attachments.Add($attachment2)

# Set up SMTP client
$smtp = New-Object Net.Mail.SmtpClient($smtpServer, 587)
$smtp.EnableSsl = $true
$smtp.Credentials = New-Object System.Net.NetworkCredential($smtpUsername, $smtpPassword)

# Send email
$smtp.Send($message)


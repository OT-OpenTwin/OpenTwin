param (
    [string]$BuildResult
)

# Set SMTP server settings
$smtpServer = "smtp.gmail.com"
$smtpFrom = "opentwin.buildauto@gmail.com"
$smtpTo = $env:OPEN_TWIN_EMAIL_LIST
$messageSubject = "OpenTwin Continuous Build $BuildResult"
$messageBody = "This is an automatically generated email from the continuous OpenTwin build." + [Environment]::NewLine + [Environment]::NewLine +"Please take a look at the attachements for more detailed information"
$smtpUsername = "opentwin.buildauto@gmail.com"
$smtpPassword = $env:OPEN_TWIN_EMAIL_PWD
$attachmentPath1 = "$env:OPENTWIN_DEV_ROOT\Scripts\BuildAndTest\buildLog_Continuous.txt"
$attachmentPath2 = "$env:OPENTWIN_DEV_ROOT\Scripts\BuildAndTest\buildLog_Summary.txt"
$attachmentPath3 = "$env:OPENTWIN_DEV_ROOT\Scripts\BuildAndTest\buildLog_Release.txt"
$attachmentPath4 = "$env:OPENTWIN_DEV_ROOT\Scripts\BuildAndTest\buildLog_Debug.txt"

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
$attachment3 = New-Object System.Net.Mail.Attachment($attachmentPath3)
$attachment4 = New-Object System.Net.Mail.Attachment($attachmentPath4)
$message.Attachments.Add($attachment1)
$message.Attachments.Add($attachment2)
$message.Attachments.Add($attachment3)
$message.Attachments.Add($attachment4)

# Set up SMTP client
$smtp = New-Object Net.Mail.SmtpClient($smtpServer, 587)
$smtp.EnableSsl = $true
$smtp.Credentials = New-Object System.Net.NetworkCredential($smtpUsername, $smtpPassword)

# Send email
$smtp.Send($message)


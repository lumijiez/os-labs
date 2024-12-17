#!/bin/bash

file_permissions_and_security() {
    local file=$1
    local directory=$2

    local file_permissions=$(stat -c "%a" "$file")
    if [ "$file_permissions" -gt 644 ]; then
        echo "The file '$file' has insecure permissions."
        chmod 644 "$file"
        echo "Permissions corrected to rw-r--r--."
    fi

    echo "Scanning directory '$directory' for .sh files..."
    find "$directory" -type f -name "*.sh" | while read -r script; do
        if [ ! -x "$script" ]; then
            echo "The script '$script' is not executable."
        else
            echo "The script '$script' is executable."
        fi
    done
}


file_integrity_checker() {
    local file=$1
    local directory=$2

    if [ ! -f "$file" ]; then
        echo "Error: File '$file' does not exist."
        return
    fi

    if [ ! -d "$directory" ]; then
        echo "Error: Directory '$directory' does not exist."
        return
    fi

    local hash=$(md5sum "$file" | awk '{print $1}')

    local filename=$(basename "$file")

    local file_found=false

    while IFS= read -r other_file; do
        local other_hash=$(md5sum "$other_file" | awk '{print $1}')

        if [ "$hash" == "$other_hash" ]; then
            echo "File integrity verified for '$other_file'."
            file_found=true
        else
            echo "File integrity compromised for '$other_file'."
        fi
    done < <(find "$directory" -type f -name "$filename")

    if [ "$file_found" == false ]; then
        echo "File '$filename' not found or not verified in the directory '$directory'."
    fi
}

disk_space_monitor() {
    current_date=$(date)
    echo "$current_date: Checking disk usage..." >> disk_usage.log
    df -h | awk -v date="$current_date" 'NR>1 && $1 != "tmpfs" {if ($5+0 > 80) print date ": Warning: Partition " $1 " mounted on " $6 " is above 80% usage."; else print date ": OK! Partition " $1 " mounted on " $6 " is fine."}' | tee -a disk_usage.log
}

system_process_tracker() {
    echo "$(date): Total used memory:" >> process_log.txt
    free -h | grep Mem | awk '{print "Used: " $3 ", Free: " $4 ", Total: " $2}' >> process_log.txt

    echo "$(date): Listing top memory-consuming processes..." >> process_log.txt
    ps aux --no-headers --sort=-%mem | head -n 5 | while read -r line; do
        pid=$(echo $line | awk '{print $2}')
        mem=$(echo $line | awk '{print $4}')
        cmd=$(echo $line | awk '{print $11}')

        echo "$(date): PID $pid, Mem: $mem%, Command: $cmd" >> process_log.txt
    done

    echo "$(date): Top 5 memory-consuming processes logged." >> process_log.txt
}

service_status_checker() {
    if [ "$#" -eq 0 ]; then
        services=("nginx" "ssh" "cron")
    else
        services=("$@")
    fi

    echo "$(date): Checking service statuses..." >> service_status.log

    for service in "${services[@]}"; do
        status=$(systemctl is-active --quiet "$service" && echo "running" || echo "not running")
        echo "$(date): Service '$service' is $status." >> service_status.log

        if [ "$status" = "not running" ]; then
            echo "$(date): Service '$service' is not running. Restarting..." | tee -a service_status.log
            systemctl restart "$service"
        fi
    done

    echo "$(date): Service status check complete." >> service_status.log
}

backup_automation() {
    local dir=$1
    local backup_dir="./backup"

    if [ ! -d "$dir" ]; then
        echo "Error: The provided directory '$dir' does not exist."
        return 1
    fi

    if [ ! -d "$backup_dir" ]; then
        mkdir -p "$backup_dir"
    fi

    local timestamp=$(date +"%Y%m%d%H%M%S")
    local backup_file="$backup_dir/$(basename "$dir")_$timestamp.tar.gz"

    tar -czf "$backup_file" "$dir" && echo "Backup created successfully at $backup_file."

    if [ $? -ne 0 ]; then
        echo "Error: Backup failed."
        return 2
    fi
}

network_connectivity_checker() {
    local targets_file=$1
    local default_targets=("8.8.8.8" "google.com" "yahoo.com")

    if [ -z "$targets_file" ]; then
        targets_file="/tmp/targets.txt"
        printf "%s\n" "${default_targets[@]}" > "$targets_file"
    fi

    echo "$(date): Checking network connectivity..." >> connectivity_log.txt

    while IFS= read -r target; do
        if ping -c 1 "$target" &>/dev/null; then
            echo "$(date): $target is reachable." | tee -a connectivity_log.txt
        else
            echo "$(date): Warning: $target is unreachable." | tee -a connectivity_log.txt
        fi
    done < "$targets_file"
}

log_cleaner() {
    local log_dir=$1
    local cleanup_log="cleanup.log"

    if [ ! -d "$log_dir" ]; then
        echo "Error: Directory '$log_dir' does not exist."
        return 1
    fi

    touch "$cleanup_log"

    find "$log_dir" -type f -name "*.log" -mtime +7 | while read -r log_file; do
        echo "Found old log: $log_file"
        read -p "Delete $log_file? (y/n): " choice
        if [[ "$choice" =~ ^[Yy]$ ]]; then
            rm "$log_file"
            echo "Deleted: $log_file" >> "$cleanup_log"
        fi
    done
}

cron_job_monitor() {
    local user=$1
    local log_file="/var/log/syslog"
    local failure_log="cron_failures.log"
    local current_date=$(date +"%b %d")

    echo "$(date): Checking cron jobs for user '$user'..." >> "$failure_log"

    crontab -l -u "$user" | while read -r job; do
        grep "$current_date" "$log_file" | grep "CRON" | grep -q "$job"

        if [ $? -ne 0 ]; then
            echo "$(date): Cron job failed: $job" >> "$failure_log"
        fi
    done
}

ram_usage_analyzer() {
    echo "$(date): Monitoring RAM usage..." >> ram_usage.log
    for i in {1..100}; do
        read total used free <<< $(free -m | awk '/Mem:/ {print $2, $3, $4}')

        used_percentage=$((used * 100 / total))

        echo "$(date): Total: ${total}MB, Used: ${used}MB, Free: ${free}MB, Used Percentage: ${used_percentage}%" >> ram_usage.log

        progress_bar="["
        for ((j = 0; j < used_percentage; j+=2)); do
            progress_bar="${progress_bar}#"
        done
        for ((j = used_percentage; j < 100; j+=2)); do
            progress_bar="${progress_bar} "
        done
        progress_bar="${progress_bar}] $used_percentage%"
        echo -e "$progress_bar"

        if [ "$used_percentage" -gt 50 ]; then
            echo "$(date): Warning: Memory usage exceeds 50%" | tee -a ram_usage.log
        fi

        sleep 1
    done
}

open_ports_scanner() {
    expected_ports=(22 80 443)

    declare -A processed_ports

    ss -tuln | awk 'NR>1 {print $5}' | while read -r port_info; do
        port="${port_info##*:}"

        if [[ -n "${processed_ports[$port]}" ]]; then
            continue
        fi

        processed_ports[$port]=1

        if [[ " ${expected_ports[@]} " =~ " ${port} " ]]; then
            service_name=$(ss -tuln | grep ":$port" | awk '{print $1, $5}')
            echo "Port $port is open and expected. Service: $service_name" >> open_ports.log
        else
            echo "Warning: Port $port is open and unexpected." >> open_ports.log
            echo "Warning: Port $port is open and unexpected."
        fi
    done
}

directory_size_auditor() {
    local dir=$1

    if [[ -z "$dir" || ! -d "$dir" ]]; then
        echo "Error: Please provide a valid directory path."
        return 1
    fi

    echo "$(date): Analyzing directory sizes in '$dir'..." >> dir_size_audit.log

    du -sh "$dir"/* 2>/dev/null | sort -rh | head -n 5 | while read -r size path; do
        echo "$(basename "$path"): $size" >> dir_size_audit.log
    done

    echo "Top 5 largest directories logged."
}

exercises=(
    "File Permissions and Security Script"
    "File Integrity Checker"
    "Disk Space Monitor"
    "System Process Tracker"
    "Service Status Checker"
    "Backup Automation Script"
    "Network Connectivity Checker"
    "Log Cleaner Script"
    "Cron Job Status Monitor"
    "RAM Usage Analyzer"
    "Open Ports Scanner"
    "Directory Size Auditor"
)

echo "Choose an exercise to run:"
for i in {1..12}; do
    echo "$i) ${exercises[$i-1]}"
done

echo "Enter the number of the exercise:"
read exercise

case $exercise in
    1)
        echo "Enter the file path:"
        read file
        echo "Enter the directory path:"
        read directory
        file_permissions_and_security "$file" "$directory"
        ;;
    2)
        echo "Enter the file path:"
        read file
        echo "Enter the directory path:"
        read directory
        file_integrity_checker "$file" "$directory"
        ;;
    3)
        disk_space_monitor
        ;;
    4)
        system_process_tracker
        ;;
    5)
        echo "Enter service names separated by spaces:"
        read -a services
        service_status_checker "${services[@]}"
        ;;
    6)
        echo "Enter the directory path to backup:"
        read dir
        backup_automation "$dir"
        ;;
    7)
        echo "Enter the path to the targets file:"
        read targets_file
        network_connectivity_checker "$targets_file"
        ;;
    8)
        echo "Enter the log directory path:"
        read log_dir
        log_cleaner "$log_dir"
        ;;
    9)
        echo "Enter the user name:"
        read user
        cron_job_monitor "$user"
        ;;
    10)
        ram_usage_analyzer
        ;;
    11)
        open_ports_scanner
        ;;
    12)
        echo "Enter the directory path:"
        read dir
        directory_size_auditor "$dir"
        ;;
    *)
        echo "Invalid choice. Exiting."
        ;;
esac